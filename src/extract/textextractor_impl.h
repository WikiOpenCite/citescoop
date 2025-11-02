// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_
#define SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_

#include <istream>
#include <memory>
#include <utility>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"

#include "base_extractor.h"

namespace wikiopencite::citescoop {

/// @brief Implementation of a text stream extractor.
class TextExtractor::TextExtractorImpl : BaseExtractor {
 public:
  /// @brief Construct a new text extractor.
  /// @param parser Citations parser to use.
  explicit TextExtractorImpl(
      std::shared_ptr<wikiopencite::citescoop::Parser> parser);

  /// @brief Extract citations from a text stream.
  /// @param stream Stream to parse.
  /// @return Pages and referenced revisions.
  std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
            std::unique_ptr<wikiopencite::proto::RevisionMap>>
  Extract(std::istream& stream);

 private:
  std::shared_ptr<wikiopencite::citescoop::Parser> parser_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_
