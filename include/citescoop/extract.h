// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INCLUDE_CITESCOOP_EXTRACT_H_
#define INCLUDE_CITESCOOP_EXTRACT_H_

#include <istream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "citescoop/citescoop_export.h"
#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

namespace wikiopencite::citescoop {

/// @brief An abstract Wikimedia XML dumps parser to parse citations.
///
/// Extractors are designed to take in the Wikimedia XML dumps in a
/// variety of formats and then return a set of their pages citations
/// and revisions.
class CITESCOOP_EXPORT Extractor {
 public:
  /// @brief Virtual destructor for inheritance
  virtual ~Extractor();

  /// @brief Extract citations from a given input stream.
  /// @param stream Input stream to extract citations from.
  /// @return A vector of citations by page and a map of revisions
  /// referenced by citations.
  virtual std::pair<
      std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
      std::unique_ptr<std::map<uint64_t, wikiopencite::proto::Revision>>>
  Extract(std::istream& stream) = 0;

  /// @brief Extract citations a given input stream.
  /// @param input XML stream to extract from.
  /// @param pages_output Output stream for pages.
  /// @param revisions_output Output stream for revisions.
  /// @return Number of pages followed by number of revisions written.
  virtual std::pair<uint64_t, uint64_t> Extract(
      std::istream& input, std::ostream* pages_output,
      std::ostream* revisions_output) = 0;
};

/// @brief Extractor for text based input streams.
class CITESCOOP_EXPORT TextExtractor : public Extractor {
 public:
  /// @brief Construct a new TextExtractor.
  /// @param parser Citations parser to use.
  explicit TextExtractor(std::shared_ptr<Parser> parser);

  ~TextExtractor() override;

  /// @brief Extract citations from text based streams.
  /// @param stream A stream of XML wikimedia dumps.
  /// @return  A vector of citations by page and a map of revisions
  /// referenced by citations.
  std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
            std::unique_ptr<std::map<uint64_t, wikiopencite::proto::Revision>>>
  Extract(std::istream& stream) override;

  /// @brief Extract citations a text based input stream.
  /// @param input XML stream to extract from.
  /// @param pages_output Output stream for pages.
  /// @param revisions_output Output stream for revisions.
  /// @return Number of pages followed by number of revisions written.
  std::pair<uint64_t, uint64_t> Extract(
      std::istream& input, std::ostream* pages_output,
      std::ostream* revisions_output) override;

 private:
  class TextExtractorImpl;
  std::unique_ptr<TextExtractorImpl> impl_;
};

/// @brief A bzip2 extractor designed to work with the bz2 Wikipedia dumps.
class CITESCOOP_EXPORT Bz2Extractor : public Extractor {
 public:
  /// @brief Construct a new bzip extractor.
  /// @param parser Citations parser to use.
  explicit Bz2Extractor(std::shared_ptr<Parser> parser);

  ~Bz2Extractor() override;

  /// @brief Extract citations from a bzip2 compressed data dump.
  /// @param stream Stream of a bzip2 compressed XML data dump.
  /// @return A vector of citations by page and a map of revisions
  /// referenced by citations.
  std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
            std::unique_ptr<std::map<uint64_t, wikiopencite::proto::Revision>>>
  Extract(std::istream& stream) override;

  /// @brief Extract citations from a bzip2 compressed data dump.
  /// @param input Stream of a bzip2 compressed XML data dump.
  /// @param pages_output Output stream for pages.
  /// @param revisions_output Output stream for revisions.
  /// @return Number of pages followed by number of revisions written.
  std::pair<uint64_t, uint64_t> Extract(
      std::istream& input, std::ostream* pages_output,
      std::ostream* revisions_output) override;

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
