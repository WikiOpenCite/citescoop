// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <string>

#include "citescoop/parser.h"

namespace wikiopencite::citescoop {
TemplateParseException::TemplateParseException(const std::string& message)
    : std::runtime_error("Citation parse failure: " + message) {}

TemplateParseException::TemplateParseException(const char* message)
    : std::runtime_error(std::string("Citation parse failure: ") + message) {}

TemplateParseException::TemplateParseException(const std::string& message,
                                               const std::string& input_text)
    : std::runtime_error("Citation parse failure: " + message + " (input: \"" +
                         // NOLINTNEXTLINE(whitespace/indent_namespace)
                         truncate_input(input_text) + "\")") {}

std::string TemplateParseException::truncate_input(const std::string& input) {
  constexpr int kMaxLength = 100;
  if (input.length() <= kMaxLength) {
    return input;
  }
  return input.substr(0, kMaxLength - 3) + "...";
}
}  // namespace wikiopencite::citescoop
