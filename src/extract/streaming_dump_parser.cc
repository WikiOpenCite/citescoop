// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "streaming_dump_parser.h"

#include <istream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"

#include "citescoop/extract.h"
#include "citescoop/io.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

StreamingDumpParser::StreamingDumpParser(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : DumpParser(parser) {}

std::pair<uint64_t, uint64_t> StreamingDumpParser::ParseXML(
    std::istream& input, std::ostream* pages_output,
    std::ostream* revisions_output) {
  page_writer_ = std::make_unique<MessageWriter>(pages_output);
  revision_writer_ = std::make_unique<MessageWriter>(revisions_output);

  StartParser(input);

  return {pages_written_, revisions_written_};
}

void StreamingDumpParser::Store(
    const std::map<uint64_t, wikiopencite::proto::Revision>& revisions,
    const wikiopencite::proto::Page& page) {
  page_writer_->WriteMessage(page);
  pages_written_++;

  for (const auto& [_, revision] : revisions) {
    revision_writer_->WriteMessage(revision);
    revisions_written_++;
  }
}

}  // namespace wikiopencite::citescoop
