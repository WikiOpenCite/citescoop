// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INCLUDE_CITESCOOP_EXTRACT_H_
#define INCLUDE_CITESCOOP_EXTRACT_H_

#include <istream>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "citescoop/citescoop_export.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision_map.pb.h"

namespace wikiopencite::citescoop {
class CITESCOOP_EXPORT Extractor {
 public:
  virtual ~Extractor();
  virtual std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
                    std::unique_ptr<wikiopencite::proto::RevisionMap>>
  Extract(std::istream& stream) = 0;
};

class CITESCOOP_EXPORT TextExtractor : public Extractor {
 public:
  explicit TextExtractor(std::shared_ptr<Parser> parser);
  ~TextExtractor() override;
  std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
            std::unique_ptr<wikiopencite::proto::RevisionMap>>
  Extract(std::istream& stream) override;

 private:
  class TextExtractorImpl;
  std::unique_ptr<TextExtractorImpl> impl_;
};

class CITESCOOP_EXPORT Bz2Extractor : public Extractor {
 public:
  explicit Bz2Extractor(std::shared_ptr<Parser> parser);
  ~Bz2Extractor() override;
  std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
            std::unique_ptr<wikiopencite::proto::RevisionMap>>
  Extract(std::istream& stream) override;

 private:
  class Bz2ExtractorImpl;
  std::unique_ptr<Bz2ExtractorImpl> impl_;
};

/// @brief Exception thrown when dump parsing fails.
///
/// This exception is thrown when the parser cannot successfully parse
/// the input dump.
class CITESCOOP_EXPORT DumpParseException : public std::runtime_error {
 public:
  /// @brief Constructs a DumpParseException with a descriptive message.
  ///
  /// @param message Description of the parse failure.
  explicit DumpParseException(const std::string& message);

  /// @brief Virtual destructor to ensure proper cleanup in inheritance
  /// hierarchies.
  ~DumpParseException() noexcept override = default;
};
}  // namespace wikiopencite::citescoop

#endif  // INCLUDE_CITESCOOP_EXTRACT_H_
