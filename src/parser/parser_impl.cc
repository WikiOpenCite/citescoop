// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parser_impl.h"

#include <string>
#include <vector>

#include "boost/algorithm/string.hpp"
#include "boost/parser/parser.hpp"

#include "citescoop/proto/extracted_citation.pb.h"
#include "citescoop/proto/revision_citations.pb.h"
#include "citescoop/proto/url.pb.h"

namespace algo = boost::algorithm;
namespace proto = wikiopencite::proto;

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

Parser::ParserImpl::ParserImpl(std::function<bool(const std::string&)> filter,
                               ParserOptions options)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : filter_(filter), options_(options) {}

proto::RevisionCitations Parser::ParserImpl::Parse(const std::string& text) {
  auto citations = proto::RevisionCitations();

  auto first = text.begin();
  auto last = text.end();
  auto const results =
      bp::prefix_parse(first, last, kWikitextRule, bp::ws, bp::trace::off);

  if (results) {
    for (const auto& result : *results) {
      auto normalised_name = algo::trim_copy(result.name);
      algo::to_lower(normalised_name);

      if (this->filter_(normalised_name)) {
        auto citation = citations.add_citations();
        this->BuildCitation(result, citation);
      }
    }

  } else {
    throw TemplateParseException("Failed to parse WikiText");
  }
  return citations;
}

void Parser::ParserImpl::BuildCitation(const TemplateEntry& entry,
                                       proto::ExtractedCitation* citation) {
  for (const auto& param : entry.params) {
    auto key = algo::trim_copy(param.key);
    algo::to_lower(key);

    if (param.value.has_value()) {
      if (key == "title") {
        citation->set_title(algo::trim_copy(param.value.value()));
      }
      // Identifiers
      // NOLINTNEXTLINE(whitespace/newline, readability/braces)
      else if (key == "doi") {
        citation->mutable_identifiers()->set_doi(
            this->ParseDoi(algo::trim_copy(param.value.value())));
      } else if (key == "isbn") {
        citation->mutable_identifiers()->set_isbn(
            algo::trim_copy(param.value.value()));
      } else if (key == "pmid") {
        try {
          citation->mutable_identifiers()->set_pmid(
              this->StrToIntIdent(algo::trim_copy(param.value.value())));
        } catch (const TemplateParseException& e) {
          if (!this->options().ignore_invalid_ident)
            throw e;
        }
      } else if (key == "pmc") {
        try {
          citation->mutable_identifiers()->set_pmcid(
              this->ParsePmcId(algo::trim_copy(param.value.value())));
        } catch (const TemplateParseException& e) {
          if (!this->options().ignore_invalid_ident)
            throw e;
        }
      } else if (key == "issn") {
        citation->mutable_identifiers()->set_issn(
            algo::trim_copy(param.value.value()));
      }
      // URLs
      // NOLINTNEXTLINE(whitespace/newline, readability/braces)
      else if (key == "url") {
        auto url_message = citation->add_urls();
        url_message->set_type(proto::UrlType::URL_TYPE_DEFAULT);
        url_message->set_url(algo::trim_copy(param.value.value()));
      } else if (key == "archive-url") {
        auto url_message = citation->add_urls();
        url_message->set_type(proto::UrlType::URL_TYPE_ARCHIVE);
        url_message->set_url(algo::trim_copy(param.value.value()));
      }
    }
  }
}

std::string Parser::ParserImpl::ParseDoi(std::string doi) {
  if (doi.starts_with("https://doi.org/")) {
    return algo::erase_first_copy(doi, "https://doi.org/");
  }
  return doi;
}

int Parser::ParserImpl::ParsePmcId(std::string pmcid) {
  if (pmcid.starts_with("PMC")) {
    return StrToIntIdent(algo::erase_first_copy(pmcid, "PMC"));
  }
  return StrToIntIdent(pmcid);
}

int Parser::ParserImpl::StrToIntIdent(std::string ident) {
  try {
    return std::stoi(ident);
  } catch (std::invalid_argument const& ex) {
    throw TemplateParseException("Failed to parse ident: " +
                                 std::string(ex.what()));
  } catch (std::out_of_range const& ex) {
    throw TemplateParseException("Failed to parse ident: " +
                                 std::string(ex.what()));
  }
}
}  // namespace wikiopencite::citescoop
