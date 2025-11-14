// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/io.h"
#include "citescoop/proto/file_header.pb.h"
#include "citescoop/proto/language.pb.h"

const std::string TEST_NAME_PREFIX = "[IO] ";

namespace cs = wikiopencite::citescoop;
namespace proto = wikiopencite::proto;

TEST_CASE(TEST_NAME_PREFIX + "Read and write with IO reader and writer",
          "[io]") {
  auto stream = std::make_shared<std::stringstream>(
      std::ios::binary | std::ios::in | std::ios::out);

  auto reader = cs::MessageReader(stream);
  auto writer = cs::MessageWriter(stream);
  stream->clear();

  auto message = proto::FileHeader();
  message.set_page_count(10);
  message.set_language(proto::Language::LANGUAGE_ENGLISH);

  auto size = writer.WriteMessage(message);
  REQUIRE(size == message.ByteSizeLong());
  REQUIRE(stream->str().size() == size + sizeof(uint32_t));

  stream->seekg(0);

  auto read_message = reader.ReadMessage<proto::FileHeader>();
  REQUIRE(read_message->page_count() == 10);
  REQUIRE(read_message->language() == proto::Language::LANGUAGE_ENGLISH);
}
