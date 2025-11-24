// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_STREAMING_DUMP_PARSER_H_
#define SRC_EXTRACT_STREAMING_DUMP_PARSER_H_

#include <istream>
#include <map>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "citescoop/io.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "dump_parser.h"

namespace wikiopencite::citescoop {

/// @brief MediaWiki XML dump parser.
class StreamingDumpParser : private DumpParser {
 public:
  /// @brief Construct a new dumps parser.
  /// @param parser The citation parser to use.
  explicit StreamingDumpParser(
      std::shared_ptr<wikiopencite::citescoop::Parser> parser);

  /// @brief Parse the dump XML.
  /// @param input An input stream of plain XML. NOTE: if you are
  /// dealing with a compressed dump, this must have already been
  /// decompressed by this point.
  /// @param pages_output Output stream for pages.
  /// @param revisions_output Output stream for revisions.
  /// @return Number of pages written to the stream followed by the
  /// number of revisions written.
  std::pair<uint64_t, uint64_t> ParseXML(std::istream& input,
                                         std::ostream* pages_output,
                                         std::ostream* revisions_output);

 protected:
  void Store(const std::map<uint64_t, wikiopencite::proto::Revision>& revisions,
             const wikiopencite::proto::Page& page) override;

 private:
  uint64_t pages_written_ = 0;
  uint64_t revisions_written_ = 0;
  std::unique_ptr<MessageWriter> page_writer_;
  std::unique_ptr<MessageWriter> revision_writer_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_STREAMING_DUMP_PARSER_H_
