// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bz2extractor_impl.h"

#include <istream>
#include <memory>
#include <tuple>
#include <utility>

#include "boost/iostreams/filter/bzip2.hpp"
#include "boost/iostreams/filtering_streambuf.hpp"

#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision_map.pb.h"

#include "base_extractor.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;
namespace bio = boost::iostreams;

Bz2Extractor::Bz2ExtractorImpl::Bz2ExtractorImpl(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : BaseExtractor(parser) {}

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          // NOLINTNEXTLINE(whitespace/indent_namespace)
          std::unique_ptr<proto::RevisionMap>>
Bz2Extractor::Bz2ExtractorImpl::Extract(std::istream& stream) {
  bio::filtering_streambuf<bio::input> in;
  in.push(bio::bzip2_decompressor());
  in.push(stream);

  std::istream decompressed_stream(&in);

  return xml_parser_.ParseXML(decompressed_stream);
}
}  // namespace wikiopencite::citescoop
