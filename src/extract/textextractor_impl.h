// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_
#define SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_

#include <istream>
#include <memory>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/revisions_group.pb.h"

#include "base_extractor.h"

namespace wikiopencite::citescoop {
class TextExtractor::TextExtractorImpl : BaseExtractor {
 public:
  explicit TextExtractorImpl(
      std::shared_ptr<wikiopencite::citescoop::Parser> parser);
  wikiopencite::proto::RevisionsGroup Extract(std::istream& stream);

 private:
  std::shared_ptr<wikiopencite::citescoop::Parser> parser_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_
