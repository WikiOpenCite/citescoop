// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <tuple>

#include "citescoop/openalex.h"

#include "snapshot_processor_impl.h"

namespace wikiopencite::citescoop::openalex {
SnapshotProcessor::SnapshotProcessor()
    : impl_(std::make_unique<SnapshotProcessorImpl>()) {}

std::tuple<Authors, Institutions, Works>
SnapshotProcessor::ProcessWorksSnapshot(std::istream& input) const {
  return impl_->ProcessWorksSnapshot(input);
}

std::tuple<uint64_t, uint64_t, uint64_t>
SnapshotProcessor::ProcessWorksSnapshot(std::istream& input,
                                        std::ostream* authors_output,
                                        std::ostream* institutions_output,
                                        std::ostream* works_output) const {
  return impl_->ProcessWorksSnapshot(input, authors_output, institutions_output,
                                     works_output);
}

SnapshotProcessor::~SnapshotProcessor() = default;

}  // namespace wikiopencite::citescoop::openalex
