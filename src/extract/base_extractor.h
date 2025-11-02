// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_BASE_EXTRACTOR_H_
#define SRC_EXTRACT_BASE_EXTRACTOR_H_

#include <memory>

#include "citescoop/parser.h"

#include "dump_parser.h"

namespace wikiopencite::citescoop {

/// @brief Base extractor implementation providing access to the dump parser.
class BaseExtractor {
 public:
  /// @brief Construct a new base extractor. Will automatically create a
  /// dump parser for you.
  /// @param citation_parser Citation parser to use.
  explicit BaseExtractor(std::shared_ptr<Parser> citation_parser)
      : citation_parser_(citation_parser),
        xml_parser_(DumpParser(citation_parser)) {}

 protected:
  /// Citation parser to use
  std::shared_ptr<Parser> citation_parser_;

  /// XML parser to use
  DumpParser xml_parser_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_BASE_EXTRACTOR_H_
