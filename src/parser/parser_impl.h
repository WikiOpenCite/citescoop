// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_PARSER_PARSER_IMPL_H_
#define SRC_PARSER_PARSER_IMPL_H_

#include <functional>
#include <string>
#include <vector>

#include "boost/parser/parser.hpp"

#include "citescoop/parser.h"
#include "citescoop/proto/extracted_citation.pb.h"

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
  std::vector<wikiopencite::proto::ExtractedCitation> Parse(
      const std::string& text);

  /// @brief Get configured parser options.
  ///
  /// @returns Parsers configuration.
  ParserOptions options() { return this->options_; }

 private:
  /// @brief Build a new @link wikiopencite::proto::ExtractedCitation
  /// from the parse result.
  ///
  /// Will iterate through the parameters of the template, extracting
  /// any that are relevant to us to construct the citation.
  ///
  /// @param entry Parser result.
  ///
  /// @return Citation including relevant parameter values.
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
  int ParsePmcId(std::string pmcid);

  /// @brief Parse an integer identifier as an int.
  ///
  /// Any exceptions caused during the conversion will be converted into
  /// TemplateParserException.
  ///
  /// @param ident Identifier to parse.
  /// @return Result
  int StrToIntIdent(std::string ident);

  /// @brief Filter function to filter citations by template type.
  std::function<bool(const std::string&)> filter_;

  /// @brief Parser configuration options.
  ParserOptions options_;
};

}  // namespace wikiopencite::citescoop

#endif  // SRC_PARSER_PARSER_IMPL_H_
