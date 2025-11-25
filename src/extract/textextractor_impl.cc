// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "textextractor_impl.h"

#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "base_extractor.h"
#include "dump_parser.h"
#include "streaming_dump_parser.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

TextExtractor::TextExtractorImpl::TextExtractorImpl(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : BaseExtractor(std::move(parser)) {}

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          // NOLINTNEXTLINE(whitespace/indent_namespace)
          std::unique_ptr<std::map<uint64_t, proto::Revision>>>
TextExtractor::TextExtractorImpl::Extract(std::istream& stream) {
  auto xml_parser = DumpParser(citation_parser_);
  return xml_parser.ParseXML(stream);
}

std::pair<uint64_t, uint64_t> TextExtractor::TextExtractorImpl::Extract(
    std::istream& input, std::ostream* pages_output,
    std::ostream* revisions_output) {
  auto xml_parser = StreamingDumpParser(citation_parser_);
  return xml_parser.ParseXML(input, pages_output, revisions_output);
}
}  // namespace wikiopencite::citescoop
