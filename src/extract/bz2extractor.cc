// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "bz2extractor_impl.h"

namespace wikiopencite::citescoop {

namespace proto = wikiopencite::proto;

Bz2Extractor::Bz2Extractor(
    std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : impl_(std::make_unique<Bz2ExtractorImpl>(parser)) {}

Bz2Extractor::~Bz2Extractor() = default;

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          std::unique_ptr<std::map<uint64_t, proto::Revision>>>
Bz2Extractor::Extract(std::istream& stream) {
  return impl_->Extract(stream);
}

std::pair<uint64_t, uint64_t> Bz2Extractor::Extract(
    std::istream& input, std::ostream* pages_output,
    std::ostream* revisions_output) {
  return impl_->Extract(input, pages_output, revisions_output);
}
}  // namespace wikiopencite::citescoop
