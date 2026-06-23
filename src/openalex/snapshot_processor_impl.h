// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_OPENALEX_SNAPSHOT_PROCESSOR_IMPL_H_
#define SRC_OPENALEX_SNAPSHOT_PROCESSOR_IMPL_H_

#include <cstdint>
#include <istream>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_set>

#include "boost/algorithm/string/erase.hpp"
#include "nlohmann/json_fwd.hpp"

#include "citescoop/openalex.h"
#include "citescoop/proto/identifiers.pb.h"
#include "citescoop/proto/openalex/work.pb.h"

namespace wikiopencite::citescoop::openalex {

/// @brief Implementation of the snapshot processor for handling
/// OpenAlex dataset snapshots
class SnapshotProcessor::SnapshotProcessorImpl {
 public:
  SnapshotProcessorImpl() = default;
  ~SnapshotProcessorImpl() = default;

  /// @brief Process an OpenAlex works snapshot
  /// @param input Input stream in JSONLines format
  /// @return The authors, institutions and works found in this snapshot
  std::tuple<Authors, Institutions, Works> ProcessWorksSnapshot(
      std::istream& input);

  /// @brief Process an OpenAlex works snapshot
  /// @param input Input stream in JSONLines format
  /// @param authors_output Output stream for authors in citescoop pbf format
  /// @param institutions_output Output stream for institutions in
  /// citescoop pbf format
  /// @param works_output Output stream for works in citescoop pbf format
  /// @return The number of messages sent to each stream (authors,
  /// institutions, works)
  std::tuple<uint64_t, uint64_t, uint64_t> ProcessWorksSnapshot(
      std::istream& input, std::ostream* authors_output,
      std::ostream* institutions_output, std::ostream* works_output);

 private:
  /// @brief Process a single JSON line from the work snapshot
  ///
  /// Will deduplicate authors and institutions based on their OpenAlex
  /// IDs and add any new ones to the seen_ids_ set.
  ///
  /// @param line Input line in JSON format representing a work
  /// @return A tuple containing the authors, institutions and work
  /// extracted from the line.
  std::tuple<Authors, Institutions, wikiopencite::proto::openalex::Work>
  ProcessLine(const std::string& line);

  /// @brief Extract work information from JSON data
  /// @param data JSON data representing a work
  /// @return The extracted work
  static wikiopencite::proto::openalex::Work ExtractWork(
      const nlohmann::json& data);

  /// @brief Extract author information from JSON data
  /// @param data JSON data representing a work
  /// @return The extracted authors
  Authors ExtractAuthors(const nlohmann::json& data);

  /// @brief Extract institution information from JSON data
  /// @param data JSON data representing a work
  /// @return The extracted institutions
  Institutions ExtractInstitutions(const nlohmann::json& data);

  /// @brief Extract identifier information from JSON data
  /// @param data JSON data representing a work
  /// @return The extracted identifiers
  static wikiopencite::proto::Identifiers ExtractIdentifiers(
      const nlohmann::json& data);

  /// @brief Remove a specific prefix from a string.
  /// @param str The input string to process.
  /// @param prefix The prefix to remove if present.
  /// @return The input string without the prefix, or the original string if
  /// the prefix was not present.
  static std::string RemovePrefix(const std::string& str,
                                  const std::string& prefix) {
    if (str.starts_with(prefix)) {
      return boost::algorithm::erase_first_copy(str, prefix);
    }
    return str;
  }

  /// @brief Trim the OpenAlex URL prefix from an OpenAlex identifier.
  /// @param openalex_id The OpenAlex identifier string, possibly prefixed
  /// with "https://openalex.org/".
  /// @return The trimmed OpenAlex identifier without the URL prefix.
  static std::string TrimOpenAlexId(const std::string& openalex_id) {
    return RemovePrefix(openalex_id, "https://openalex.org/");
  }

  /// @brief Parse a PubMed identifier URL into a numeric PubMed ID.
  /// @param pubmed_id The PubMed identifier string, possibly prefixed with
  /// "https://pubmed.ncbi.nlm.nih.gov/".
  /// @return The parsed PubMed identifier as a uint32_t.
  static uint32_t ParsePubmedId(const std::string& pubmed_id) {
    return static_cast<uint32_t>(std::stoul(
        RemovePrefix(pubmed_id, "https://pubmed.ncbi.nlm.nih.gov/")));
  }

  /// @brief Get the institution ID from an institution JSON object
  ///
  /// If no OpenAlex ID exist for the institution, it will be identified
  /// as follows:
  ///   - With an ROR: "UNKNOWN_<ror>"
  ///   - Without an ROR: "UNKNOWN_<display name>"
  ///
  /// @param institution Institution JSON record
  /// @return OpenAlex identifier (shortened) or the fallback alternatives
  static std::string GetInstitutionId(const nlohmann::json& institution);

  /// @brief Get the author ID from an author JSON object
  ///
  /// If no OpenAlex ID exist for the author, it will be identified
  /// as follows:
  ///   - With an orcid: "UNKNOWN_<orcid>"
  ///   - Without an orcid: "UNKNOWN_<display name>"
  ///
  /// @param author Orcid JSON record
  /// @return OpenAlex identifier (shortened) or the fallback alternatives
  static std::string GetAuthorId(const nlohmann::json& author);

  /// @brief Convert an OpenAlex category string into the corresponding
  /// Work::OACategory enum value.
  /// @param category The OpenAlex category string.
  /// @return The matching enum value for the given category.
  static wikiopencite::proto::openalex::Work::OACategory StringToOaCategory(
      const std::string& category);

  /// @brief Set of author and institution IDs already seen (so
  /// shouldn't be included again)
  std::unordered_set<std::string> seen_ids_;
};
}  // namespace wikiopencite::citescoop::openalex

#endif  // SRC_OPENALEX_SNAPSHOT_PROCESSOR_IMPL_H_
