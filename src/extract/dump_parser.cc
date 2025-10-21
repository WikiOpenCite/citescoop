// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dump_parser.h"

#include <istream>
#include <memory>

#include "citescoop/proto/revisions_group.pb.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

DumpParser::DumpParser(std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : parser_(parser) {}

proto::RevisionsGroup DumpParser::ParseXML(std::istream& stream) {
  char buf[64];
  const size_t buf_size = sizeof(buf) / sizeof(char);

  set_substitute_entities(true);
  do {
    std::memset(buf, 0, buf_size);
    stream.read(buf, buf_size - 1);
    if (stream.gcount()) {
      xmlpp::ustring input(buf, buf + stream.gcount());
    }
  } while (stream);

  finish_chunk_parsing();

  return wikiopencite::proto::RevisionsGroup();
}

}  // namespace wikiopencite::citescoop
