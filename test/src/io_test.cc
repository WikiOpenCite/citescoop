// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdint>
#include <ios>
#include <sstream>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/io.h"
#include "citescoop/proto/file_header.pb.h"
#include "citescoop/proto/language.pb.h"

const std::string kTestNamePrefix = "[IO] ";

namespace cs = wikiopencite::citescoop;
namespace proto = wikiopencite::proto;

TEST_CASE(kTestNamePrefix + "Read and write with IO reader and writer",
          "[io]") {
  auto stream =
      std::stringstream(std::ios::binary | std::ios::in | std::ios::out);

  auto reader = cs::MessageReader(&stream);
  auto writer = cs::MessageWriter(&stream);
  stream.clear();

  auto message = proto::FileHeader();
  // NOLINTNEXTLINE(readability-magic-numbers)
  message.set_count(10);
  message.set_type(proto::FileType::FILE_TYPE_OPENALEX_AUTHORS);

  auto size = writer.WriteMessage(message);
  REQUIRE(size == message.ByteSizeLong());
  REQUIRE(stream.str().size() == size + sizeof(uint32_t));

  stream.seekg(0);

  auto read_message = reader.ReadMessage<proto::FileHeader>();
  REQUIRE(read_message->count() == 10);
  REQUIRE(read_message->type() == proto::FileType::FILE_TYPE_OPENALEX_AUTHORS);
}
