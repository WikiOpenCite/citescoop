// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <string>

#include "citescoop/parser.h"

#include <catch2/catch_test_macros.hpp>

namespace cs = wikiopencite::citescoop;

/// Check that the parser can handle successfully extract the title from
/// a citation.
TEST_CASE("Single citation with title", "[parser]") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result = parser.parse("{{cite journal | title=Parsing in Practice}}");

  REQUIRE(result.size() == 1);
  auto citation = result.at(0);

  REQUIRE(citation.has_title());
  REQUIRE(citation.title() == "Parsing in Practice");
  REQUIRE(!citation.has_identifiers());
  REQUIRE(result.at(0).urls_size() == 0);
}

/// Ensure that DOI formats are always in the short form (i.e. missing
/// the https://doi.org/ prefix).
TEST_CASE("Consistent DOI formats", "[parser]") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result1 = parser.parse("{{cite journal | doi=10.1007/b62130}}");
  auto citation1 = result1.at(0);
  REQUIRE(citation1.identifiers().doi() == "10.1007/b62130");

  auto result2 =
      parser.parse("{{cite journal | doi=https://doi.org/10.1007/b62130}}");
  auto citation2 = result2.at(0);
  REQUIRE(citation2.identifiers().doi() == "10.1007/b62130");
}

/// Check that identifiers can be correctly extracted, and where
/// required cast.
TEST_CASE("Extract identifiers", "[parser]") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result = parser.parse(
      "{{cite journal | doi=10.1007/b62130 | isbn=0-786918-50-0 | "
      "pmid=17322060 | pmc=345678 | issn=2049-3630}}");

  auto citation = result.at(0);
  REQUIRE(citation.has_identifiers());
  auto identifiers = citation.identifiers();

  REQUIRE(identifiers.doi() == "10.1007/b62130");
  REQUIRE(identifiers.isbn() == "0-786918-50-0");
  REQUIRE(identifiers.pmid() == 17322060);
  REQUIRE(identifiers.pmcid() == 345678);
  REQUIRE(identifiers.issn() == "2049-3630");
}

/// Check the parser can correctly cast a PMC ID containing the PMC
/// prefix to an integer.
TEST_CASE("PMC ID containing PMC prefix") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result = parser.parse("{{cite journal|pmc = PMC345678}}");
  auto citation = result.at(0);

  REQUIRE(citation.identifiers().pmcid() == 345678);
}

/// Check that the parser can correctly throw / not throw an exception
/// on invalid numerical idents dependent upon configuration.
TEST_CASE("Numeric identifiers that cannot be cast", "[parser]") {
  SECTION("throw an exception") {
    // We expect the default to be throwing an exception
    auto parser_throws = cs::Parser();
    REQUIRE_THROWS_AS(parser_throws.parse("{{cite journal|pmc = abc123}}"),
                      cs::TemplateParseException);
    REQUIRE_THROWS_AS(parser_throws.parse("{{cite journal|pmid = abc123}}"),
                      cs::TemplateParseException);

    // Int too big
    REQUIRE_THROWS_AS(parser_throws.parse("{{cite journal|pmid = 2147483648}}"),
                      cs::TemplateParseException);
  }
  SECTION("ignore invalid idents") {
    cs::ParserOptions options{.ignore_invalid_ident = true};
    auto parser_no_throw = cs::Parser(options);

    auto result =
        parser_no_throw.parse("{{cite journal|pmc = abc123|pmid=abc123}}");
    auto citation = result.at(0);

    REQUIRE_FALSE(citation.identifiers().has_pmid());
    REQUIRE_FALSE(citation.identifiers().has_pmcid());
  }
}

/// Ensure that the parser can handle additional whitespace around the
/// template.
TEST_CASE("Additional whitespace", "[parser]") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result = parser.parse(
      "{{    cite    journal   |   title = Parsing in Practice }}");
  auto citation = result.at(0);

  REQUIRE(citation.has_title());
  REQUIRE(citation.title() == "Parsing in Practice");
}

/// Ensure that the parser can handle minimum whitespace around the
/// template.
TEST_CASE("Minimum whitespace", "[parser]") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result = parser.parse("{{cite journal|title = Parsing in Practice}}");
  auto citation = result.at(0);

  REQUIRE(citation.has_title());
  REQUIRE(citation.title() == "Parsing in Practice");
}

/// Ensure the parser can extract multiple citations from a block of WikiText.
TEST_CASE("Multiple citations in text block", "[parser]") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result = parser.parse(
      "Urban beekeeping, the practice of keeping bee colonies in towns and "
      "cities, has grown in popularity over recent years due to increasing "
      "awareness of pollinator decline and interest in sustainable food "
      "practices.<ref name=\" Guardian2015"
      "\">{{cite news |last=Helm |first=Toby |title=Urban beekeeping is all "
      "the "
      "buzz in London "
      "|url=https://www.theguardian.com/environment/2015/may/10/"
      "urban-beekeeping-london-honeybees |work=The Guardian |date=10 May 2015 "
      "|access-date=5 August 2025}}</ref> Advocates argue that city "
      "environments can actually be beneficial to bees, offering a more "
      "diverse and less pesticide-intensive range of flora than many rural "
      "areas.<ref name=\" BBC2020"
      "\">{{cite web |title=Why city bees are healthier than country bees "
      "|url=https://www.bbc.com/future/article/"
      "20200207-why-city-bees-are-healthier-than-country-bees |website=BBC "
      "Future |date=7 February 2020 |access-date=5 August 2025}}</ref> "
      "However, critics warn that high hive densities in urban areas may lead "
      "to increased competition for forage and the spread of disease.<ref "
      "name=\" Nature2018"
      "\">{{Citation |last=Jones |first=Imogen |title=Urban beekeeping: An "
      "analysis of ecological sustainability |journal=Nature Ecology & "
      "Evolution |volume=2 |issue=8 |pages=1245–1247 |year=2018 "
      "|doi=10.1038/s41559-018-0602-5}}</ref>");

  REQUIRE(result.size() == 3);
}

/// Check that we can correctly set and retrieve parser options.
TEST_CASE("Get options", "[parser]") {
  SECTION("Get default options no filter") {
    auto parser = cs::Parser();
    auto options = parser.getOptions();

    REQUIRE_FALSE(options.ignore_invalid_ident);
  }

  SECTION("Get default options with filter") {
    auto parser = cs::Parser([](auto) { return true; });
    auto options = parser.getOptions();

    REQUIRE_FALSE(options.ignore_invalid_ident);
  }

  SECTION("Set custom options no filter") {
    cs::ParserOptions options = {.ignore_invalid_ident = true};
    auto parser = cs::Parser(options);

    REQUIRE(parser.getOptions().ignore_invalid_ident);
  }

  SECTION("Set custom options with filter") {
    cs::ParserOptions options = {.ignore_invalid_ident = true};
    auto parser = cs::Parser([](auto) { return true; }, options);

    REQUIRE(parser.getOptions().ignore_invalid_ident);
  }
}
