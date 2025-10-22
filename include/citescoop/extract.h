// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INCLUDE_CITESCOOP_EXTRACT_H_
#define INCLUDE_CITESCOOP_EXTRACT_H_

#include <istream>
#include <memory>
#include <vector>

#include "citescoop/citescoop_export.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"

namespace wikiopencite::citescoop {
class CITESCOOP_EXPORT Extractor {
 public:
  virtual ~Extractor();
  virtual std::unique_ptr<std::vector<wikiopencite::proto::Page>> Extract(
      std::istream& stream) = 0;
};

class CITESCOOP_EXPORT TextExtractor : public Extractor {
 public:
  explicit TextExtractor(std::shared_ptr<Parser> parser);
  ~TextExtractor();
  std::unique_ptr<std::vector<wikiopencite::proto::Page>> Extract(
      std::istream& stream) override;

 private:
  class TextExtractorImpl;
  std::unique_ptr<TextExtractorImpl> impl_;
};

class CITESCOOP_EXPORT Bz2Extractor : public Extractor {
 public:
  explicit Bz2Extractor(std::shared_ptr<Parser> parser);
  ~Bz2Extractor();
  std::unique_ptr<std::vector<wikiopencite::proto::Page>> Extract(
      std::istream& stream) override;

 private:
  class Bz2ExtractorImpl;
  std::unique_ptr<Bz2ExtractorImpl> impl_;
};
}  // namespace wikiopencite::citescoop

#endif  // INCLUDE_CITESCOOP_EXTRACT_H_
