// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_BASE_EXTRACTOR_H_
#define SRC_EXTRACT_BASE_EXTRACTOR_H_

#include <memory>

#include "citescoop/parser.h"

#include "dump_parser.h"

namespace wikiopencite::citescoop {
class BaseExtractor {
 public:
  explicit BaseExtractor(std::shared_ptr<Parser> citation_parser)
      : citation_parser_(citation_parser),
        xml_parser_(DumpParser(citation_parser)) {}

 protected:
  std::shared_ptr<Parser> citation_parser_;
  DumpParser xml_parser_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_BASE_EXTRACTOR_H_
