// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "textextractor_impl.h"

#include <istream>
#include <memory>
#include <utility>

#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"

#include "base_extractor.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

TextExtractor::TextExtractorImpl::TextExtractorImpl(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : BaseExtractor(parser) {}

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          // NOLINTNEXTLINE(whitespace/indent_namespace)
          std::unique_ptr<proto::RevisionMap>>
TextExtractor::TextExtractorImpl::Extract(std::istream& stream) {
  return xml_parser_.ParseXML(stream);
}
}  // namespace wikiopencite::citescoop
