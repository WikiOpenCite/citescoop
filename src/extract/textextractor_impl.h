// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_
#define SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_

#include <istream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

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
            std::unique_ptr<std::map<uint64_t, wikiopencite::proto::Revision>>>
  Extract(std::istream& stream);

  /// @brief Extract implementation.
  /// @param input Input text stream.
  /// @param pages_output Output stream for pages.
  /// @param revisions_output Output stream for revisions.
  /// @return The number of pages written, then the number of revisions written.
  std::pair<uint64_t, uint64_t> Extract(
      std::istream& input, std::shared_ptr<std::ostream> pages_output,
      std::shared_ptr<std::ostream> revisions_output);

 private:
  std::shared_ptr<wikiopencite::citescoop::Parser> parser_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_TEXTEXTRACTOR_IMPL_H_
