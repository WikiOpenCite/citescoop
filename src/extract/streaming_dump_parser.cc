// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "streaming_dump_parser.h"

#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <utility>

#include "citescoop/io.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "dump_parser.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

StreamingDumpParser::StreamingDumpParser(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : DumpParser(std::move(parser)) {}

std::pair<uint64_t, uint64_t> StreamingDumpParser::ParseXML(
    std::istream& input, std::ostream* pages_output,
    std::ostream* revisions_output) {
  page_writer_ = std::make_unique<MessageWriter>(pages_output);
  revision_writer_ = std::make_unique<MessageWriter>(revisions_output);

  StartParser(input);

  return {pages_written_, revisions_written_};
}

void StreamingDumpParser::Store(
    const std::map<uint64_t, proto::Revision>& revisions,
    const proto::Page& page) {
  page_writer_->WriteMessage(page);
  pages_written_++;

  for (const auto& [unused, revision] : revisions) {
    revision_writer_->WriteMessage(revision);
    revisions_written_++;
  }
}

}  // namespace wikiopencite::citescoop
