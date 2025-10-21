// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "citescoop/parser.h"

#include <string>
#include <vector>

#include "parser_impl.h"

namespace wikiopencite::citescoop {
Parser::Parser() : Parser([](auto) { return true; }) {}

Parser::Parser(std::function<bool(const std::string&)> filter)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : Parser(filter, ParserOptions()) {}

Parser::Parser(ParserOptions options)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : Parser([](auto) { return true; }, options) {}

Parser::Parser(std::function<bool(const std::string&)> filter,
               ParserOptions options)
    : impl_(std::make_unique<ParserImpl>(filter, options)) {}

Parser::~Parser() = default;

wikiopencite::proto::RevisionCitations Parser::Parse(const std::string& text) {
  return this->impl_->Parse(text);
}

ParserOptions Parser::options() {
  return this->impl_->options();
}
}  // namespace wikiopencite::citescoop
