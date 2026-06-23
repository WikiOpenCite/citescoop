// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INCLUDE_CITESCOOP_OPENALEX_H_
#define INCLUDE_CITESCOOP_OPENALEX_H_

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <tuple>
#include <vector>

#include "citescoop/citescoop_export.h"
#include "citescoop/proto/openalex/author.pb.h"
#include "citescoop/proto/openalex/institution.pb.h"
#include "citescoop/proto/openalex/work.pb.h"

namespace wikiopencite::citescoop::openalex {

using Authors = std::vector<wikiopencite::proto::openalex::Author>;
using Institutions = std::vector<wikiopencite::proto::openalex::Institution>;
using Works = std::vector<wikiopencite::proto::openalex::Work>;

/// @brief Process OpenAlex dataset snapshots
///
/// Will extract the fields we require for linking to citations and
/// return them as protocol buffers for easy (and smaller) storage. This
/// avoids having to store the entire OpenAlex dataset.
///
/// Two processing methods are available, in memory and streaming direct
/// to an output stream.
class CITESCOOP_EXPORT SnapshotProcessor {
 public:
  SnapshotProcessor();
  ~SnapshotProcessor();

  /// @brief Extract works, authors and institutions from an OpenAlex
  /// snapshot
  ///
  /// Authors and institutions will be deduplicated based on their
  /// OpenAlex IDs.
  ///
  /// @param input Input stream in JSONLines format.
  /// @return The authors, institutions and works found in this snapshot.
  std::tuple<Authors, Institutions, Works> ProcessWorksSnapshot(
      std::istream& input) const;

  /// @brief Extract works, authors and institutions from an OpenAlex
  /// snapshot
  ///
  /// Streaming based approach to processing, will write protobuf messages
  /// directly to output stream instead of returning them as a vector.
  /// Authors and institutions will be deduplicated in the output streams.
  ///
  /// @param input Input stream in JSONLines format
  /// @param authors_output Output stream for authors in citescoop pbf format.
  /// @param institutions_output Output stream for institutions in
  /// citescoop pbf format.
  /// @param works_output Output stream for works in citescoop pbf format.
  /// @return The number of messages sent to each stream (authors,
  /// institutions, works).
  std::tuple<uint64_t, uint64_t, uint64_t> ProcessWorksSnapshot(
      std::istream& input, std::ostream* authors_output,
      std::ostream* institutions_output, std::ostream* works_output) const;

 private:
  class SnapshotProcessorImpl;
  std::unique_ptr<SnapshotProcessorImpl> impl_;
};

}  // namespace wikiopencite::citescoop::openalex

#endif  // INCLUDE_CITESCOOP_OPENALEX_H_
