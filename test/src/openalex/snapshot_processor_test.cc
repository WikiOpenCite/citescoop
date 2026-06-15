// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/openalex.h"
#include "citescoop/proto/language.pb.h"
#include "citescoop/proto/openalex/work.pb.h"
#include "google/protobuf/util/time_util.h"

#include "util.h"  // NOLINT(misc-include-cleaner)

const std::string kTestNamePrefix = "[SnapshotProcessor] ";

namespace proto = wikiopencite::proto;
namespace openalex = wikiopencite::citescoop::openalex;

TEST_CASE(kTestNamePrefix + "SnapshotProcessor can be constructed",
          "[openalex][openalex/SnapshotProcessor]") {
  auto processor = openalex::SnapshotProcessor();
  REQUIRE(true);
}

TEST_CASE(kTestNamePrefix + "Can process single line OpenAlex snapshot",
          "[openalex][openalex/SnapshotProcessor]") {
  auto processor = openalex::SnapshotProcessor();

  std::ifstream file(
      FILE("data/single-line.jsonl"));  // NOLINT(misc-include-cleaner)
  REQUIRE(file.is_open());

  auto [authors, institutions, works] = processor.ProcessWorksSnapshot(file);

  REQUIRE(authors.size() == 1);
  REQUIRE(institutions.size() == 1);
  REQUIRE(works.size() == 1);

  auto author = authors.at(0);
  REQUIRE(author.openalex_id() == "A123456789");
  REQUIRE(author.name() == "John Doe");
  REQUIRE(author.orcid() == "0000-0001-2345-6789");

  auto institution = institutions.at(0);
  REQUIRE(institution.openalex_id() == "I123456789");
  REQUIRE(institution.name() == "Example University");
  REQUIRE(institution.ror() == "https://ror.org/012345678");

  auto work = works.at(0);
  REQUIRE(work.openalex_id() == "W1234567890");
  REQUIRE(work.ids().doi() == "10.1234/example.doi");
  REQUIRE(work.ids().pmid() == 12345678);
  auto timestamp = google::protobuf::Timestamp();
  google::protobuf::util::TimeUtil::FromString("2023-01-01", &timestamp);
  REQUIRE(work.publication_date().seconds() == timestamp.seconds());
  REQUIRE(work.publication_date().nanos() == timestamp.nanos());
  REQUIRE(work.language() == proto::Language::LANGUAGE_ENGLISH);
  REQUIRE(work.oa_status() ==
          proto::openalex::Work_OACategory_OA_CATEGORY_GOLD);
  REQUIRE(work.oa_url() == "https://example.com/oa");
  REQUIRE(work.author_ids_size() == 1);
  REQUIRE(work.author_ids(0) == "A123456789");
  REQUIRE(work.institution_ids_size() == 1);
  REQUIRE(work.institution_ids(0) == "I123456789");
  REQUIRE(work.title() == "Sample Work Title");
}

TEST_CASE(kTestNamePrefix + "Can process with missing / null fields",
          "[openalex][openalex/SnapshotProcessor]") {
  auto processor = openalex::SnapshotProcessor();

  std::ifstream file(FILE("data/missing-fields.jsonl"));
  REQUIRE(file.is_open());

  auto [authors, institutions, works] = processor.ProcessWorksSnapshot(file);

  REQUIRE(authors.size() == 1);
  REQUIRE(institutions.size() == 1);
  REQUIRE(works.size() == 16);
}

TEST_CASE(kTestNamePrefix + "Can process with additional whitespace",
          "[openalex][openalex/SnapshotProcessor]") {
  auto processor = openalex::SnapshotProcessor();

  std::ifstream file(FILE("data/extra-whitespace.jsonl"));
  REQUIRE(file.is_open());

  auto [authors, institutions, works] = processor.ProcessWorksSnapshot(file);

  REQUIRE(authors.size() == 1);
  REQUIRE(institutions.size() == 1);
  REQUIRE(works.size() == 16);
}
