// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_PARSER_PARSER_IMPL_H_
#define SRC_PARSER_PARSER_IMPL_H_

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "citescoop/parser.h"
#include "citescoop/proto/extracted_citation.pb.h"
#include "citescoop/proto/revision_citations.pb.h"

namespace wikiopencite::citescoop {

struct ParameterEntry {
  std::string key;
  std::optional<std::string> value;
};

struct TemplateEntry {
  std::string name;
  std::vector<ParameterEntry> params;
};

class Parser::ParserImpl {
 public:
  explicit ParserImpl(std::function<bool(const std::string&)> filter,
                      ParserOptions options);

  /// @brief Parse a given WikiText input, applying the filter for each
  /// citation extracted.
  ///
  /// @param text WikiText input.
  ///
  /// @return List of the extracted citations.
  wikiopencite::proto::RevisionCitations Parse(const std::string& text);

  /// @brief Get configured parser options.
  ///
  /// @returns Parsers configuration.
  ParserOptions options() { return this->options_; }

 private:
  /// @brief Build a @link wikiopencite::proto::ExtractedCitation
  /// from the parse result.
  ///
  /// Will iterate through the parameters of the template, extracting
  /// any that are relevant to us to construct the citation.
  ///
  /// @param entry Parser result.
  wikiopencite::proto::ExtractedCitation BuildCitation(
      const TemplateEntry& entry);

  /// @brief Parse a DOI into it's short form.
  ///
  /// Will remove the https://doi.org/ prefix if it is present.
  ///
  /// @param doi DOI to parse.
  /// @return Normalized DOI.
  std::string ParseDoi(std::string doi);

  /// @brief Parse the PMC Id.
  ///
  /// Parse the PMC Id, removing the PMC prefix as required. If the
  /// resulting number cannot be converted to an int, a @link
  /// wikiopencite::citescoop::TemplateParserException @endlink will be
  /// thrown.
  ///
  /// @param pmcid PMC Id to parse.
  /// @return Resulting PMC Id.
  int ParsePmcId(const std::string& pmcid);

  /// @brief Parse an integer identifier as an int.
  ///
  /// Any exceptions caused during the conversion will be converted into
  /// TemplateParserException.
  ///
  /// @param ident Identifier to parse.
  /// @return Result
  int StrToIntIdent(const std::string& ident);

  /// @brief Check if the key is for an identifier. If so, add to the
  /// citation.
  /// @param citation Citation to modify if the key contains an identifier.
  /// @param key Name of key.
  /// @param value Value of key.
  /// @return Has an update been made?
  bool CheckForIdentKey(wikiopencite::proto::ExtractedCitation* citation,
                        const std::string& key, const std::string& value);

  /// @brief Check if the key is for a URL. If so, add to the
  /// citation.
  /// @param citation Citation to modify if the key contains an identifier.
  /// @param key Name of key.
  /// @param value Value of key.
  /// @return Has an update been made?
  bool CheckForUrlKey(wikiopencite::proto::ExtractedCitation* citation,
                      const std::string& key, const std::string& value);

  /// @brief Handle setting the PMC ID for a citation.
  /// Will attempt to parse the PMC ID. If it cannot parse the PMC ID it
  /// will either throw an exception or if the ignore invalid
  /// identifiers is set, will simply ignore.
  /// @param citation Citation to modify.
  /// @param value Value of PMC ID key.
  /// @return Has an update been made?
  bool HandlePmcIdKey(wikiopencite::proto::ExtractedCitation* citation,
                      const std::string& value);

  /// @brief Handle setting the PM ID for a citation.
  /// Will attempt to parse the PM ID. If it cannot parse the PM ID it
  /// will either throw an exception or if the ignore invalid
  /// identifiers is set, will simply ignore.
  /// @param citation Citation to modify.
  /// @param value Value of PM ID key.
  /// @return Has an update been made?
  bool HandlePmIdKey(wikiopencite::proto::ExtractedCitation* citation,
                     const std::string& value);

  /// @brief Filter function to filter citations by template type.
  std::function<bool(const std::string&)> filter_;

  /// @brief Parser configuration options.
  ParserOptions options_;
};

}  // namespace wikiopencite::citescoop

#endif  // SRC_PARSER_PARSER_IMPL_H_
