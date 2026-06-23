// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "snapshot_processor_impl.h"

#include <cstdint>
#include <istream>
#include <ostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"
#include "nlohmann/json.hpp"

#include "citescoop/io.h"
#include "citescoop/openalex.h"
#include "citescoop/proto/openalex/author.pb.h"
#include "citescoop/proto/openalex/institution.pb.h"
#include "citescoop/proto/openalex/work.pb.h"
#include "langmap.h"

namespace wikiopencite::citescoop::openalex {

namespace {
namespace citescoop = wikiopencite::citescoop;
using json = nlohmann::json;  // NOLINT(misc-include-cleaner)

}  // namespace

std::tuple<Authors, Institutions, Works>
SnapshotProcessor::SnapshotProcessorImpl::ProcessWorksSnapshot(
    std::istream& input) {
  Authors authors = std::vector<wikiopencite::proto::openalex::Author>();
  Institutions institutions =
      std::vector<wikiopencite::proto::openalex::Institution>();
  Works works = std::vector<wikiopencite::proto::openalex::Work>();

  std::string line;
  while (std::getline(input, line)) {
    if (!line.empty()) {
      auto [line_authors, line_institutions, line_work] = ProcessLine(line);
      authors.insert(authors.end(), line_authors.begin(), line_authors.end());
      institutions.insert(institutions.end(), line_institutions.begin(),
                          line_institutions.end());
      works.push_back(line_work);
    }
  }

  return std::tuple<Authors, Institutions, Works>(authors, institutions, works);
}

std::tuple<uint64_t, uint64_t, uint64_t>
SnapshotProcessor::SnapshotProcessorImpl::ProcessWorksSnapshot(
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    std::istream& input, std::ostream* authors_output,
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    std::ostream* institutions_output, std::ostream* works_output) {  //
  uint64_t authors_count = 0;
  uint64_t institutions_count = 0;
  uint64_t works_count = 0;

  citescoop::MessageWriter authors_writer(authors_output);
  citescoop::MessageWriter institutions_writer(institutions_output);
  citescoop::MessageWriter works_writer(works_output);

  std::string line;
  while (std::getline(input, line)) {
    if (!line.empty()) {
      auto [line_authors, line_institutions, line_work] = ProcessLine(line);
      for (const auto& author : line_authors) {
        authors_writer.WriteMessage(author);
        authors_count++;
      }
      for (const auto& institution : line_institutions) {
        institutions_writer.WriteMessage(institution);
        institutions_count++;
      }
      works_writer.WriteMessage(line_work);
      works_count++;
    }
  }

  return std::tuple<uint64_t, uint64_t, uint64_t>(
      authors_count, institutions_count, works_count);
}

std::tuple<Authors, Institutions, proto::openalex::Work>
SnapshotProcessor::SnapshotProcessorImpl::ProcessLine(const std::string& line) {
  const json kData = json::parse(line);

  return std::tuple<Authors, Institutions, proto::openalex::Work>(
      ExtractAuthors(kData), ExtractInstitutions(kData), ExtractWork(kData));
}

proto::openalex::Work SnapshotProcessor::SnapshotProcessorImpl::ExtractWork(
    const nlohmann::json& data) {
  proto::openalex::Work work;

  work.set_openalex_id(TrimOpenAlexId(data["id"].get<std::string>()));
  if (data.contains("title") && data["title"].is_string()) {
    work.set_title(data["title"].get<std::string>());
  }

  if (data.contains("publication_date") &&
      data["publication_date"].is_string()) {
    auto timestamp = google::protobuf::Timestamp();
    google::protobuf::util::TimeUtil::FromString(
        data["publication_date"].get<std::string>(), &timestamp);
    work.mutable_publication_date()->CopyFrom(timestamp);
  }

  if (data.contains("language") && data["language"].is_string()) {
    work.set_language(
        citescoop::ISOToLanguage(data["language"].get<std::string>()));
  }

  work.set_oa_status(
      StringToOaCategory(data["open_access"]["oa_status"].get<std::string>()));

  // Always exists, but might be null
  if (data["open_access"]["oa_url"].is_string()) {
    work.set_oa_url(data["open_access"]["oa_url"].get<std::string>());
  }

  // Get the authors and institutions IDs from the authorships field,
  // deduplicating as needed.
  std::set<std::string> seen_institution_ids;
  for (const auto& author : data["authorships"]) {
    // No need to deduplicate authors, as they are already deduplicated
    // by OpenAlex.
    if (!IsAuthorEmpty(author["author"])) {
      work.add_author_ids(GetAuthorId(author["author"]));

      for (const auto& institution : author["institutions"]) {
        if (!IsInstitutionEmpty(institution)) {
          const std::string kTrimmedInstitutionId =
              GetInstitutionId(institution);

          if (!seen_institution_ids.contains(kTrimmedInstitutionId)) {
            seen_institution_ids.insert(kTrimmedInstitutionId);
            work.add_institution_ids(kTrimmedInstitutionId);
          }
        }
      }
    }
  }

  work.mutable_ids()->CopyFrom(ExtractIdentifiers(data));

  return work;
}

Authors SnapshotProcessor::SnapshotProcessorImpl::ExtractAuthors(
    const nlohmann::json& data) {

  auto authors = Authors();

  for (const auto& author : data["authorships"]) {
    if (!IsAuthorEmpty(author["author"])) {
      std::string author_id = GetAuthorId(author["author"]);

      if (!seen_ids_.contains(author_id)) {
        seen_ids_.insert(author_id);

        proto::openalex::Author author_message;
        author_message.set_openalex_id(author_id);

        if (author["author"].contains("display_name") &&
            author["author"]["display_name"].is_string()) {
          author_message.set_name(
              author["author"]["display_name"].get<std::string>());
        }

        if (author["author"].contains("orcid") &&
            author["author"]["orcid"].is_string()) {
          author_message.set_orcid(
              author["author"]["orcid"].get<std::string>());
        }

        authors.push_back(author_message);
      }
    }
  }
  return authors;
}

Institutions SnapshotProcessor::SnapshotProcessorImpl::ExtractInstitutions(
    const nlohmann::json& data) {
  auto institutions = Institutions();

  for (const auto& author : data["authorships"]) {
    for (const auto& institution : author["institutions"]) {
      if (!IsInstitutionEmpty(institution)) {
        std::string institution_id = GetInstitutionId(institution);

        if (!seen_ids_.contains(institution_id)) {
          seen_ids_.insert(institution_id);

          proto::openalex::Institution institution_message;
          institution_message.set_openalex_id(institution_id);
          if (institution.contains("display_name") &&
              institution["display_name"].is_string()) {
            institution_message.set_name(
                institution["display_name"].get<std::string>());
          }

          if (institution.contains("ror") && institution["ror"].is_string()) {
            institution_message.set_ror(institution["ror"].get<std::string>());
          }

          institutions.push_back(institution_message);
        }
      }
    }
  }

  return institutions;
}

proto::Identifiers SnapshotProcessor::SnapshotProcessorImpl::ExtractIdentifiers(
    const nlohmann::json& data) {
  proto::Identifiers identifiers;

  if (data["ids"].contains("doi") && data["ids"]["doi"].is_string()) {
    identifiers.set_doi(RemovePrefix(data["ids"]["doi"].get<std::string>(),
                                     "https://doi.org/"));
  }

  if (data["ids"].contains("pmid") && data["ids"]["pmid"].is_string()) {
    identifiers.set_pmid(ParsePubmedId(data["ids"]["pmid"].get<std::string>()));
  }

  // TODO(@Computroniks): Add more identifiers as needed

  return identifiers;
}

std::string SnapshotProcessor::SnapshotProcessorImpl::GetInstitutionId(
    const nlohmann::json& institution) {
  if (institution.contains("id") && institution["id"].is_string()) {
    return TrimOpenAlexId(institution["id"].get<std::string>());
  }

  if (institution.contains("ror") && institution["ror"].is_string()) {
    return "UNKNOWN_" + institution["ror"].get<std::string>();
  }

  return "UNKNOWN_" + institution["display_name"].get<std::string>();
}

std::string SnapshotProcessor::SnapshotProcessorImpl::GetAuthorId(
    const nlohmann::json& author) {
  if (author.contains("id") && author["id"].is_string()) {
    return TrimOpenAlexId(author["id"].get<std::string>());
  }

  if (author.contains("orcid") && author["orcid"].is_string()) {
    return "UNKNOWN_" + author["orcid"].get<std::string>();
  }

  return "UNKNOWN_" + author["display_name"].get<std::string>();
}

wikiopencite::proto::openalex::Work::OACategory
SnapshotProcessor::SnapshotProcessorImpl::StringToOaCategory(
    const std::string& category) {
  if (category == "diamond") {
    return wikiopencite::proto::openalex::Work_OACategory_OA_CATEGORY_DIAMOND;
  }
  if (category == "gold") {
    return wikiopencite::proto::openalex::Work_OACategory_OA_CATEGORY_GOLD;
  }
  if (category == "green") {
    return wikiopencite::proto::openalex::Work_OACategory_OA_CATEGORY_GREEN;
  }
  if (category == "hybrid") {
    return wikiopencite::proto::openalex::Work_OACategory_OA_CATEGORY_HYBRID;
  }
  if (category == "bronze") {
    return wikiopencite::proto::openalex::Work_OACategory_OA_CATEGORY_BRONZE;
  }
  if (category == "closed") {
    return wikiopencite::proto::openalex::Work_OACategory_OA_CATEGORY_CLOSED;
  }
  return wikiopencite::proto::openalex::Work_OACategory_OA_CATEGORY_UNSPECIFIED;
}

bool SnapshotProcessor::SnapshotProcessorImpl::IsInstitutionEmpty(
    const nlohmann::json& institution) {

  const bool kMissingId =
      !(institution.contains("id") && institution["id"].is_string());
  const bool kMissingName = !(institution.contains("display_name") &&
                              institution["display_name"].is_string());
  const bool kMissingRor =
      !(institution.contains("ror") && institution["ror"].is_string());

  return kMissingId && kMissingName && kMissingRor;
}

bool SnapshotProcessor::SnapshotProcessorImpl::IsAuthorEmpty(
    const nlohmann::json& author) {
  const bool kMissingId = !(author.contains("id") && author["id"].is_string());
  const bool kMissingName =
      !(author.contains("display_name") && author["display_name"].is_string());
  const bool kMissingOrcid =
      !(author.contains("ror") && author["ror"].is_string());

  return kMissingId && kMissingName && kMissingOrcid;
}

}  // namespace wikiopencite::citescoop::openalex
