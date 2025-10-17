// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parser_impl.h"

#include <string>
#include <vector>

#include "boost/algorithm/string.hpp"
#include "boost/parser/parser.hpp"

#include "citescoop/proto/extracted_citation.pb.h"

namespace algo = boost::algorithm;

namespace wikiopencite::citescoop {

namespace {
namespace bp = boost::parser;

// Rules
const bp::rule<class template_type_r, std::string> kTemplateTypeRule =
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    "template_type";
const bp::rule<class key_r, std::string> kKeyRule = "key";
const bp::rule<class val_r, std::string> kValRule = "value";
const bp::rule<class parameter_r, ParameterEntry> kParamRule = "parameter";
const bp::rule<class template_r, TemplateEntry> kTemplateRule = "template";
const bp::rule<class wikitext_r, std::vector<TemplateEntry>> kWikitextRule =
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    "wikitext";

// Definitions
auto const kTemplateTypeRule_def = bp::lexeme[+(bp::char_ - '|')];
auto const kKeyRule_def = bp::lexeme[*(bp::char_ - '=' - '|' - '}')];
auto const kValRule_def = '=' >> bp::lexeme[*(bp::char_ - '|' - '}')];
auto const kParamRule_def = kKeyRule >> -kValRule;
auto const kTemplateRule_def = bp::lit("{{") >> kTemplateTypeRule >> '|' >>
                               // NOLINTNEXTLINE(whitespace/indent_namespace)
                               kParamRule % '|' >> bp::lit("}}");

// Skip any characters until the next "{{" without consuming the "{{"
auto const kSkipToTemplate = *(!bp::lit("{{") >> bp::char_);

// Repeat: skip junk then parse a template
auto const kWikitextRule_def = *(bp::omit[kSkipToTemplate] >> kTemplateRule);

BOOST_PARSER_DEFINE_RULES(kTemplateTypeRule, kKeyRule, kValRule, kParamRule,
                          kTemplateRule, kWikitextRule);
}  // namespace

Parser::ParserImpl::ParserImpl(std::function<bool(const std::string&)> filter)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : filter_(filter) {}

std::vector<wikiopencite::proto::ExtractedCitation> Parser::ParserImpl::parse(
    const std::string& text) {
  std::vector<wikiopencite::proto::ExtractedCitation> citations = {};

  auto first = text.begin();
  auto last = text.end();
  auto const results =
      bp::prefix_parse(first, last, kWikitextRule, bp::ws, bp::trace::off);

  if (results) {
    for (const auto& result : *results) {
      auto normalised_name = algo::trim_copy(result.name);
      algo::to_lower(normalised_name);

      if (this->filter_(normalised_name)) {
        citations.push_back(this->buildCitation(result));
      }
    }

  } else {
    throw TemplateParseException("Failed to parse WikiText");
  }
  return citations;
}

wikiopencite::proto::ExtractedCitation Parser::ParserImpl::buildCitation(
    const TemplateEntry& entry) {

  auto citation = wikiopencite::proto::ExtractedCitation();

  for (const auto& param : entry.params) {
    auto key = algo::trim_copy(param.key);
    algo::to_lower(key);

    if (param.value.has_value()) {
      if (key == "title") {
        citation.set_title(algo::trim_copy(param.value.value()));
      }
    }
  }

  return citation;
}
}  // namespace wikiopencite::citescoop
