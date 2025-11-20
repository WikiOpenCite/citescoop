// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>
#include <iostream>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/extract.h"
#include "citescoop/io.h"

#include "util.h"

const std::string TEST_NAME_PREFIX = "[BZ2 Extractor] ";

namespace cs = wikiopencite::citescoop;
namespace proto = wikiopencite::proto;

/// Check that the extractor can handle extracting a single citation
/// from a single page containing a single revision.
TEST_CASE(TEST_NAME_PREFIX + "Extract single citation from single revision",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::Bz2Extractor(parser);

  std::ifstream file(
      GetTestFilePath("single-revision-single-citation.xml.bz2"));
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

  auto revision = pair.second->at(5);
  REQUIRE(revision.revision_id() == 5);
}

/// Check that the extractor handles streaming correctly.
TEST_CASE(TEST_NAME_PREFIX + "Streaming input / output",
          "[extract][extract/Extractor]") {
  auto parser = std::make_shared<cs::Parser>();
  auto extractor = cs::Bz2Extractor(parser);

  auto pages_stream = std::make_shared<std::stringstream>(
      std::ios::binary | std::ios::in | std::ios::out);
  auto page_reader = cs::MessageReader(pages_stream);
  pages_stream->clear();

  auto revisions_stream = std::make_shared<std::stringstream>(
      std::ios::binary | std::ios::in | std::ios::out);
  auto revision_reader = cs::MessageReader(revisions_stream);
  revisions_stream->clear();

  std::ifstream file(
      GetTestFilePath("single-revision-single-citation.xml.bz2"));
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
