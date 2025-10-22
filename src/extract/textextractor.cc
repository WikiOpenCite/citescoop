// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <istream>
#include <memory>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"

#include "textextractor_impl.h"

namespace wikiopencite::citescoop {

namespace proto = wikiopencite::proto;

TextExtractor::TextExtractor(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : impl_(std::make_unique<TextExtractorImpl>(parser)) {}

TextExtractor::~TextExtractor() = default;

std::unique_ptr<std::vector<wikiopencite::proto::Page>> TextExtractor::Extract(
    std::istream& stream) {
  return impl_->Extract(stream);
}

}  // namespace wikiopencite::citescoop
