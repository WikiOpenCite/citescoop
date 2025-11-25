// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "citescoop/parser.h"

#include <functional>
#include <memory>
#include <string>

#include "citescoop/proto/revision_citations.pb.h"

#include "parser_impl.h"

namespace wikiopencite::citescoop {
Parser::Parser() : Parser([](const auto&) { return true; }) {}

Parser::Parser(const std::function<bool(const std::string&)>& filter)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : Parser(filter, ParserOptions()) {}

Parser::Parser(ParserOptions options)
    // NOLINTNEXTLINE(whitespace/indent_namespace)
    : Parser([](const auto&) { return true; }, options) {}

Parser::Parser(const std::function<bool(const std::string&)>& filter,
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
