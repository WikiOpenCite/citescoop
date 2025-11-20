// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/extract.h"
#include "citescoop/io.h"
#include "citescoop/parser.h"

#include "util.h"

const std::string TEST_NAME_PREFIX = "[Extractor] ";

namespace cs = wikiopencite::citescoop;
namespace proto = wikiopencite::proto;

/// Check that the extractor can handle extracting a single citation
/// from a single page containing a single revision.
TEST_CASE(TEST_NAME_PREFIX + "Extract single citation from single revision",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("single-revision-single-citation.xml"));
  REQUIRE(file.is_open());

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);
  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == 5);
  REQUIRE_FALSE(citation.has_revision_removed());

  REQUIRE(pair.second->size() == 1);
  auto revision = pair.second->at(5);
  REQUIRE(revision.revision_id() == 5);
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

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == 5);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed() == 7);

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(5);
  REQUIRE(revision1.revision_id() == 5);
  auto revision2 = pair.second->at(7);
  REQUIRE(revision2.revision_id() == 7);
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

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == 5);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed() == 6);

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(5);
  REQUIRE(revision1.revision_id() == 5);
  auto revision2 = pair.second->at(6);
  REQUIRE(revision2.revision_id() == 6);
}

/// Check that the extractor, when presented with two revisions of the
/// same timestamp, will just use the order in which they appear.
TEST_CASE(TEST_NAME_PREFIX + "Multiple revisions with same timestamp",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-revision-same-timestamp.xml"));
  REQUIRE(file.is_open());

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == 6);
  REQUIRE_FALSE(citation.has_revision_removed());

  REQUIRE(pair.second->size() == 1);

  auto revision = pair.second->at(6);
  REQUIRE(revision.revision_id() == 6);
}

/// Check that the order of revisions is not determined by ID and is
/// instead ordered by timestamp.
TEST_CASE(TEST_NAME_PREFIX + "Order not determined by ID",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-revision-order-not-by-id.xml"));
  REQUIRE(file.is_open());

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == 6);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed() == 5);

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(5);
  REQUIRE(revision1.revision_id() == 5);
  auto revision2 = pair.second->at(6);
  REQUIRE(revision2.revision_id() == 6);
}

/// Check the extractor can correctly handle multiple pages.
TEST_CASE(TEST_NAME_PREFIX + "Multiple pages", "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-pages.xml"));
  REQUIRE(file.is_open());

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 2);

  auto page1 = result->at(0);
  REQUIRE(page1.title() == "My Page");
  REQUIRE(page1.page_id() == 1);
  REQUIRE(page1.citations_size() == 1);

  auto citation1 = page1.citations().at(0);
  REQUIRE(citation1.has_revision_added());
  REQUIRE(citation1.revision_added() == 5);
  REQUIRE_FALSE(citation1.has_revision_removed());

  auto page2 = result->at(1);
  REQUIRE(page2.title() == "My Second Page");
  REQUIRE(page2.page_id() == 2);
  REQUIRE(page2.citations_size() == 1);

  auto citation2 = page2.citations().at(0);
  REQUIRE(citation2.has_revision_added());
  REQUIRE(citation2.revision_added() == 8);
  REQUIRE_FALSE(citation2.has_revision_removed());

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(5);
  REQUIRE(revision1.revision_id() == 5);
  auto revision2 = pair.second->at(8);
  REQUIRE(revision2.revision_id() == 8);
}

/// Check the extractor will not include revisions in the revision map
/// that are not referenced.
TEST_CASE(TEST_NAME_PREFIX + "Orphaned revision inclusion",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("orphaned-revision-included.xml"));
  REQUIRE(file.is_open());

  auto pair = extractor.Extract(file);
  auto revisions = std::move(pair.second);
  REQUIRE(revisions->size() == 1);
  auto revision = revisions->at(5);
  REQUIRE(revision.revision_id() == 5);
}

/// Check the extractor can correctly throw an error when an issue occurs.
TEST_CASE(TEST_NAME_PREFIX + "Malformed XML", "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("malformed.xml"));
  REQUIRE(file.is_open());

  REQUIRE_THROWS_AS(extractor.Extract(file), cs::DumpParseException);
}

/// Check that the extractor handles streaming correctly.
TEST_CASE(TEST_NAME_PREFIX + "Streaming input / output",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  auto pages_stream = std::make_shared<std::stringstream>(
      std::ios::binary | std::ios::in | std::ios::out);
  auto page_reader = cs::MessageReader(pages_stream);
  pages_stream->clear();

  auto revisions_stream = std::make_shared<std::stringstream>(
      std::ios::binary | std::ios::in | std::ios::out);
  auto revision_reader = cs::MessageReader(revisions_stream);
  revisions_stream->clear();

  std::ifstream file(GetTestFilePath("single-revision-single-citation.xml"));
  REQUIRE(file.is_open());

  auto pair = extractor.Extract(file, pages_stream, revisions_stream);
  REQUIRE(pair.first == 1);
  REQUIRE(pair.second == 1);

  pages_stream->clear();
  pages_stream->seekg(0);
  revisions_stream->clear();
  revisions_stream->seekg(0);

  auto page = page_reader.ReadMessage<proto::Page>();
  REQUIRE(page->title() == "My Page");
  REQUIRE(page->page_id() == 1);
  REQUIRE(page->citations_size() == 1);

  auto citation = page->citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == 5);
  REQUIRE_FALSE(citation.has_revision_removed());

  auto revision = revision_reader.ReadMessage<proto::Revision>();
  REQUIRE(revision->revision_id() == 5);
}
