// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INCLUDE_CITESCOOP_PARSER_H_
#define INCLUDE_CITESCOOP_PARSER_H_

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "citescoop/citescoop_export.h"
#include "citescoop/proto/extracted_citation.pb.h"

namespace wikiopencite::citescoop {

struct CITESCOOP_EXPORT ParserOptions {
  /// @brief Should invalid identifiers be ignored?
  ///
  /// If set, the parser will not throw an exception if it can't process
  /// an identifier. E.g. if it encounters abc123 for pmid (which needs
  /// to be numeric), this identifier will be ignored and not included
  /// in the resulting citation.
  bool ignore_invalid_ident = false;
};

/// @brief A WikiText parser to extract citations, optionally filtering by
/// citation template type.
///
/// @example
/// @code
/// // Create a parser that only accepts "book" citations
/// auto filter = [](const std::string& type) {
///     return type == "citation book";
/// };
/// Parser parser(filter);
/// auto result = parser.parse("{{cite book | title=Parsing in Practice | "
///   "author=Jones | journal=Computer Science Review | year=2022 | bool }}");
/// // Result is a vector containing a single citation.
/// @endcode
class CITESCOOP_EXPORT Parser {
 public:
  /// @brief Construct a parser that has no filter and no options.
  ///
  /// Parsers with no filter will return all citations no matter their type.
  Parser();

  /// @brief Construct a new parser with a filter to filter on the type of
  /// citation.
  ///
  /// Parsers may optionally filter based upon the citation type they
  /// encounter. This may be useful if you wish to exclude all citation
  /// templates that aren't @c "citation book" for example.
  ///
  /// @param filter Filter function to apply to citation types. The
  /// function is passed a normalized string (lower case, additional
  /// whitespace removed) of the first argument in a WikiText citation
  /// (see <a
  /// href="https://en.wikipedia.org/wiki/Wikipedia:Citation_templates">
  /// Wikipedia - Citation Templates</a> for more details).
  ///
  /// @example
  /// @code
  /// // Create a parser that only accepts "book" citations
  /// auto filter = [](const std::string& type) {
  ///     return type == "citation book";
  /// };
  /// Parser parser(filter);
  /// @endcode
  explicit Parser(std::function<bool(const std::string&)> filter);

  /// @brief Construct a new parser with parser options and no filter.
  ///
  /// @param options Parser options to configure parser with.
  explicit Parser(ParserOptions options);

  /// @brief Construct a new parser with a filter and parser options.
  ///
  /// Parsers may optionally filter based upon the citation type they
  /// encounter. This may be useful if you wish to exclude all citation
  /// templates that aren't @c "citation book" for example.
  ///
  /// @param filter Filter function to apply to citation types. The
  /// function is passed a normalized string (lower case, additional
  /// whitespace removed) of the first argument in a WikiText citation
  /// (see <a
  /// href="https://en.wikipedia.org/wiki/Wikipedia:Citation_templates">
  /// Wikipedia - Citation Templates</a> for more details).
  /// @param options Parser options to configure parser with.
  Parser(std::function<bool(const std::string&)> filter, ParserOptions options);

  ~Parser();

  /// @brief Parse a given input string to extract citations.
  ///
  /// If a filter has been set for this parser, the return values will
  /// only contain any citations that match the provided filter.
  ///
  /// @param text WikiText to extract citations from.
  /// @return Citation protobuf representations.
  ///
  /// @sa Parser(std::function<bool(const std::string&)> filter)
  std::vector<wikiopencite::proto::ExtractedCitation> parse(
      const std::string& text);

  /// @brief Get configured parser options.
  /// @return Configured parser options.
  ParserOptions getOptions();

 private:
  class ParserImpl;
  std::unique_ptr<ParserImpl> impl_;
};

/// @brief Exception thrown when citation parsing fails.
///
/// This exception is thrown when the parser cannot successfully parse
/// the input text for templates, typically due to malformed wikitext
/// or syntax errors in the input.
class CITESCOOP_EXPORT TemplateParseException : public std::runtime_error {
 public:
  /// @brief Constructs a TemplateParseException with a descriptive message.
  ///
  /// @param message Description of the parse failure.
  explicit TemplateParseException(const std::string& message);

  /// @brief Constructs a TemplateParseException with a descriptive message.
  ///
  /// @param message Description of the parse failure.
  explicit TemplateParseException(const char* message);

  /// @brief Constructs a TemplateParseException with input context.
  ///
  /// @param message Description of the parse failure.
  /// @param input_text The text that failed to parse (truncated if too
  /// long).
  TemplateParseException(const std::string& message,
                         const std::string& input_text);

  /// @brief Virtual destructor to ensure proper cleanup in inheritance
  /// hierarchies.
  ~TemplateParseException() noexcept override = default;

 private:
  /// @brief Truncates input text for error messages to avoid overly long
  /// exceptions.
  ///
  /// @param input The input text to potentially truncate.
  ///
  /// @return std::string Truncated input (max 100 characters with "..."
  /// if truncated).
  static std::string truncate_input(const std::string& input);
};

}  // namespace wikiopencite::citescoop

#endif  // INCLUDE_CITESCOOP_PARSER_H_
