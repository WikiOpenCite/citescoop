// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_BZ2EXTRACTOR_IMPL_H_
#define SRC_EXTRACT_BZ2EXTRACTOR_IMPL_H_

#include <istream>
#include <memory>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"

#include "base_extractor.h"

namespace wikiopencite::citescoop {
class Bz2Extractor::Bz2ExtractorImpl : BaseExtractor {
 public:
  explicit Bz2ExtractorImpl(
      std::shared_ptr<wikiopencite::citescoop::Parser> parser);
  std::unique_ptr<std::vector<wikiopencite::proto::Page>> Extract(
      std::istream& stream);

 private:
  std::shared_ptr<wikiopencite::citescoop::Parser> parser_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_BZ2EXTRACTOR_IMPL_H_
