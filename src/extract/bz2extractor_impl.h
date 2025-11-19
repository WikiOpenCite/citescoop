// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_BZ2EXTRACTOR_IMPL_H_
#define SRC_EXTRACT_BZ2EXTRACTOR_IMPL_H_

#include <istream>
#include <map>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "citescoop/extract.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

#include "base_extractor.h"

namespace wikiopencite::citescoop {

/// @brief Implementation for the bzip extractor.
class Bz2Extractor::Bz2ExtractorImpl : BaseExtractor {
 public:
  /// @brief Create a new bzip extractor.
  /// @param parser Citations parser to use.
  explicit Bz2ExtractorImpl(
      std::shared_ptr<wikiopencite::citescoop::Parser> parser);

  /// @brief Extract implementation. This will decompress the input
  /// stream and pass it off to the XML parser.
  ///
  /// @param stream Input compressed bzip stream.
  /// @return Pages and the referenced revisions.
  std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
            std::unique_ptr<std::map<uint64_t, wikiopencite::proto::Revision>>>
  Extract(std::istream& stream);
};

}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_BZ2EXTRACTOR_IMPL_H_
