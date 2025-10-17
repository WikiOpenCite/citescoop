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
  explicit ParserImpl(std::function<bool(const std::string&)> filter);

  /// @brief Parse a given WikiText input, applying the filter for each
  /// citation extracted.
  ///
  /// @param text WikiText input.
  ///
  /// @return List of the extracted citations.
  std::vector<wikiopencite::proto::ExtractedCitation> parse(
      const std::string& text);

 private:
  /// @brief Filter function to filter citations by template type.
  std::function<bool(const std::string&)> filter_;

  /// @brief Build a new @link wikiopencite::proto::ExtractedCitation
  /// from the parse result.
  ///
  /// Will iterate through the parameters of the template, extracting
  /// any that are relevant to us to construct the citation.
  ///
  /// @param entry Parser result.
  ///
  /// @return Citation including relevant parameter values.
  wikiopencite::proto::ExtractedCitation buildCitation(
      const TemplateEntry& entry);
};

}  // namespace wikiopencite::citescoop

#endif  // SRC_PARSER_PARSER_IMPL_H_
