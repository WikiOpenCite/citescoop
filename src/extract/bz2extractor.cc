// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <memory>
#include <utility>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"

#include "bz2extractor_impl.h"

namespace wikiopencite::citescoop {

namespace proto = wikiopencite::proto;

Bz2Extractor::Bz2Extractor(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : impl_(std::make_unique<Bz2ExtractorImpl>(parser)) {}

Bz2Extractor::~Bz2Extractor() = default;

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          std::unique_ptr<proto::RevisionMap>>
Bz2Extractor::Extract(std::istream& stream) {
  return impl_->Extract(stream);
}

}  // namespace wikiopencite::citescoop
