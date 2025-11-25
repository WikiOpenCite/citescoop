// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/extract.h"
#include "citescoop/io.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "util.h"

const std::string kTestNamePrefix = "[Extractor] ";

namespace cs = wikiopencite::citescoop;
namespace proto = wikiopencite::proto;

/// Check that the extractor can handle extracting a single citation
/// from a single page containing a single revision.
TEST_CASE(kTestNamePrefix + "Extract single citation from single revision",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("single-revision-single-citation.xml"));
  REQUIRE(file.is_open());

  const int kRevisionAdded = 5;

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);
  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == kRevisionAdded);
  REQUIRE_FALSE(citation.has_revision_removed());

  REQUIRE(pair.second->size() == 1);
  auto revision = pair.second->at(kRevisionAdded);
  REQUIRE(revision.revision_id() == kRevisionAdded);
}

/// Check that the extractor can handle a citation being added in one
/// revision and then being removed in another has the added and removed
/// fields correctly set.
TEST_CASE(kTestNamePrefix + "Multiple revisions with citation being removed",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-revision-citation-removed.xml"));
  REQUIRE(file.is_open());

  const int kRevisionAdded = 5;
  const int kRevisionRemoved = 7;

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == kRevisionAdded);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed() == kRevisionRemoved);

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(kRevisionAdded);
  REQUIRE(revision1.revision_id() == kRevisionAdded);
  auto revision2 = pair.second->at(kRevisionRemoved);
  REQUIRE(revision2.revision_id() == kRevisionRemoved);
}

/// Check that the extractor can correctly handle revisions appearing in
/// a non-chronological order.
TEST_CASE(kTestNamePrefix + "Multiple revisions in non-chronological order",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(
      GetTestFilePath("multiple-revision-not-chronological.xml"));
  REQUIRE(file.is_open());

  const int kRevisionAdded = 5;
  const int kRevisionRemoved = 6;

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == kRevisionAdded);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed() == kRevisionRemoved);

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(kRevisionAdded);
  REQUIRE(revision1.revision_id() == kRevisionAdded);
  auto revision2 = pair.second->at(kRevisionRemoved);
  REQUIRE(revision2.revision_id() == kRevisionRemoved);
}

/// Check that the extractor, when presented with two revisions of the
/// same timestamp, will just use the order in which they appear.
TEST_CASE(kTestNamePrefix + "Multiple revisions with same timestamp",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-revision-same-timestamp.xml"));
  REQUIRE(file.is_open());

  const int kRevisionAdded = 6;

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == kRevisionAdded);
  REQUIRE_FALSE(citation.has_revision_removed());

  REQUIRE(pair.second->size() == 1);

  auto revision = pair.second->at(kRevisionAdded);
  REQUIRE(revision.revision_id() == kRevisionAdded);
}

/// Check that the order of revisions is not determined by ID and is
/// instead ordered by timestamp.
TEST_CASE(kTestNamePrefix + "Order not determined by ID",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-revision-order-not-by-id.xml"));
  REQUIRE(file.is_open());

  const int kRevisionAdded = 6;
  const int kRevisionRemoved = 5;

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 1);

  auto page = result->at(0);
  REQUIRE(page.title() == "My Page");
  REQUIRE(page.page_id() == 1);
  REQUIRE(page.citations_size() == 1);

  auto citation = page.citations().at(0);
  REQUIRE(citation.has_revision_added());
  REQUIRE(citation.revision_added() == kRevisionAdded);
  REQUIRE(citation.has_revision_removed());
  REQUIRE(citation.revision_removed() == kRevisionRemoved);

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(kRevisionRemoved);
  REQUIRE(revision1.revision_id() == kRevisionRemoved);
  auto revision2 = pair.second->at(kRevisionAdded);
  REQUIRE(revision2.revision_id() == kRevisionAdded);
}

/// Check the extractor can correctly handle multiple pages.
TEST_CASE(kTestNamePrefix + "Multiple pages", "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("multiple-pages.xml"));
  REQUIRE(file.is_open());

  const int kPage1RevisionAdded = 5;
  const int kPage2RevisionAdded = 8;

  auto pair = extractor.Extract(file);
  auto result = std::move(pair.first);

  REQUIRE(result->size() == 2);

  auto page1 = result->at(0);
  REQUIRE(page1.title() == "My Page");
  REQUIRE(page1.page_id() == 1);
  REQUIRE(page1.citations_size() == 1);

  auto citation1 = page1.citations().at(0);
  REQUIRE(citation1.has_revision_added());
  REQUIRE(citation1.revision_added() == kPage1RevisionAdded);
  REQUIRE_FALSE(citation1.has_revision_removed());

  auto page2 = result->at(1);
  REQUIRE(page2.title() == "My Second Page");
  REQUIRE(page2.page_id() == 2);
  REQUIRE(page2.citations_size() == 1);

  auto citation2 = page2.citations().at(0);
  REQUIRE(citation2.has_revision_added());
  REQUIRE(citation2.revision_added() == kPage2RevisionAdded);
  REQUIRE_FALSE(citation2.has_revision_removed());

  REQUIRE(pair.second->size() == 2);

  auto revision1 = pair.second->at(kPage1RevisionAdded);
  REQUIRE(revision1.revision_id() == kPage1RevisionAdded);
  auto revision2 = pair.second->at(kPage2RevisionAdded);
  REQUIRE(revision2.revision_id() == kPage2RevisionAdded);
}

/// Check the extractor will not include revisions in the revision map
/// that are not referenced.
TEST_CASE(kTestNamePrefix + "Orphaned revision inclusion",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("orphaned-revision-included.xml"));
  REQUIRE(file.is_open());

  const int kRevisionId = 5;

  auto pair = extractor.Extract(file);
  auto revisions = std::move(pair.second);
  REQUIRE(revisions->size() == 1);
  auto revision = revisions->at(kRevisionId);
  REQUIRE(revision.revision_id() == kRevisionId);
}

/// Check the extractor can correctly throw an error when an issue occurs.
TEST_CASE(kTestNamePrefix + "Malformed XML", "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  std::ifstream file(GetTestFilePath("malformed.xml"));
  REQUIRE(file.is_open());

  REQUIRE_THROWS_AS(extractor.Extract(file), cs::DumpParseException);
}

/// Check that the extractor handles streaming correctly.
TEST_CASE(kTestNamePrefix + "Streaming input / output",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::TextExtractor(parser);

  auto pages_stream =
      std::stringstream(std::ios::binary | std::ios::in | std::ios::out);
  auto page_reader = cs::MessageReader(&pages_stream);
  pages_stream.clear();

  auto revisions_stream =
      std::stringstream(std::ios::binary | std::ios::in | std::ios::out);
  auto revision_reader = cs::MessageReader(&revisions_stream);
  revisions_stream.clear();

  std::ifstream file(GetTestFilePath("single-revision-single-citation.xml"));
  REQUIRE(file.is_open());

  auto pair = extractor.Extract(file, &pages_stream, &revisions_stream);
  REQUIRE(pair.first == 1);
  REQUIRE(pair.second == 1);

  pages_stream.clear();
  pages_stream.seekg(0);
  revisions_stream.clear();
  revisions_stream.seekg(0);

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
