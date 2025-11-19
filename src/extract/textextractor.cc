// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <istream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "textextractor_impl.h"

namespace wikiopencite::citescoop {

namespace proto = wikiopencite::proto;

TextExtractor::TextExtractor(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : impl_(std::make_unique<TextExtractorImpl>(parser)) {}

TextExtractor::~TextExtractor() = default;

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          std::unique_ptr<std::map<uint64_t, proto::Revision>>>
TextExtractor::Extract(std::istream& stream) {
  return impl_->Extract(stream);
}

}  // namespace wikiopencite::citescoop
