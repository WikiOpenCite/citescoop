// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <string>

#include "citescoop/parser.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Single basic citation", "[parser]") {
  auto parser = wikiopencite::citescoop::Parser();

  auto result = parser.parse(
      "{{cite journal | title=Parsing in Practice | author=Jones | "
      "journal=Computer Science Review | year=2022 | bool }}");

  REQUIRE(result.size() == 1);
  REQUIRE(result.at(0).has_title());
  REQUIRE(result.at(0).title() == "Parsing in Practice");
  REQUIRE(!result.at(0).has_identifiers());
  REQUIRE(result.at(0).urls_size() == 0);
}

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
