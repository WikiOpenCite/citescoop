// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bz2extractor_impl.h"

#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include "boost/iostreams/categories.hpp"
#include "boost/iostreams/filter/bzip2.hpp"
#include "boost/iostreams/filtering_streambuf.hpp"
#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "base_extractor.h"
#include "dump_parser.h"
#include "streaming_dump_parser.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;
namespace bio = boost::iostreams;

Bz2Extractor::Bz2ExtractorImpl::Bz2ExtractorImpl(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : BaseExtractor(std::move(parser)) {}

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          // NOLINTNEXTLINE(whitespace/indent_namespace)
          std::unique_ptr<std::map<uint64_t, proto::Revision>>>
Bz2Extractor::Bz2ExtractorImpl::Extract(std::istream& stream) {
  bio::filtering_streambuf<bio::input> decompression_stream;
  decompression_stream.push(bio::bzip2_decompressor());
  decompression_stream.push(stream);

  std::istream decompressed_stream(&decompression_stream);

  auto xml_parser = DumpParser(citation_parser_);
  return xml_parser.ParseXML(decompressed_stream);
}

std::pair<uint64_t, uint64_t> Bz2Extractor::Bz2ExtractorImpl::Extract(
    std::istream& input, std::ostream* pages_output,
    std::ostream* revisions_output) {
  bio::filtering_streambuf<bio::input> decompression_stream;
  decompression_stream.push(bio::bzip2_decompressor());
  decompression_stream.push(input);

  std::istream decompressed_stream(&decompression_stream);
  auto xml_parser = StreamingDumpParser(citation_parser_);
  return xml_parser.ParseXML(decompressed_stream, pages_output,
                             revisions_output);
}
}  // namespace wikiopencite::citescoop
