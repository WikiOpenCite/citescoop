// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <iostream>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/extract.h"
#include "citescoop/parser.h"

const std::string TEST_NAME_PREFIX = "[Extractor] ";

namespace cs = wikiopencite::citescoop;
namespace proto = wikiopencite::proto;

static std::string GetTestFilePath(const std::string& filename) {
  // __FILE__ gives the current source file path
  std::string path = __FILE__;
  auto pos = path.find_last_of("/\\");
  return path.substr(0, pos + 1) + "data/" + filename;
}

/// Check that the extractor can handle extracting a single citation
/// from a single page containing a single revision.
TEST_CASE(TEST_NAME_PREFIX + "Extract single citation from single revision",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("single-revision-single-citation.xml"));
  REQUIRE(file.is_open());

  auto result = extractor.Extract(file);
  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added().revision_id() == 5);
  REQUIRE_FALSE(citation.has_revision_removed());
}

/// Check that the extractor can handle a citation being added in one
/// revision and then being removed in another has the added and removed
/// fields correctly set.
TEST_CASE(TEST_NAME_PREFIX + "Multiple revisions with citation being removed",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-revision-citation-removed.xml"));
  REQUIRE(file.is_open());

  auto result = extractor.Extract(file);
  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added().revision_id() == 5);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed().revision_id() == 6);
}

/// Check that the extractor can correctly handle revisions appearing in
/// a non-chronological order.
TEST_CASE(TEST_NAME_PREFIX + "Multiple revisions in non-chronological order",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(
      GetTestFilePath("multiple-revision-not-chronological.xml"));
  REQUIRE(file.is_open());

  auto result = extractor.Extract(file);
  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added().revision_id() == 5);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed().revision_id() == 6);
}

/// Check that the order of revisions is not determined by ID and is
/// instead ordered by timestamp.
TEST_CASE(TEST_NAME_PREFIX + "Order not determined by ID",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-revision-order-not-by-id.xml"));
  REQUIRE(file.is_open());

  auto result = extractor.Extract(file);
  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added().revision_id() == 6);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed().revision_id() == 5);
}

/// Check the extractor can correctly handle multiple pages.
TEST_CASE(TEST_NAME_PREFIX + "Multiple pages", "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-pages.xml"));
  REQUIRE(file.is_open());

  auto result = extractor.Extract(file);
  REQUIRE(result->size() == 1);

  auto page1 = result->at(0);
  REQUIRE(page1.title() == "My Page");
  REQUIRE(page1.page_id() == 1);
  REQUIRE(page1.citations_size() == 1);

  auto citation1 = page1.citations().at(0);
  REQUIRE(citation1.has_revision_added());
  REQUIRE_FALSE(citation1.revision_added().revision_id() == 5);
  REQUIRE(citation1.has_revision_removed());

  auto page2 = result->at(0);
  REQUIRE(page2.title() == "My Second Page");
  REQUIRE(page2.page_id() == 1);
  REQUIRE(page2.citations_size() == 1);

  auto citation2 = page2.citations().at(1);
  REQUIRE(citation2.has_revision_added());
  REQUIRE(citation2.revision_added().revision_id() == 8);
  REQUIRE_FALSE(citation2.has_revision_removed());
}
