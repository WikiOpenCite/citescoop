// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "citescoop/parser.h"

#include <string>
#include <vector>

#include "parser_impl.h"

namespace wikiopencite::citescoop {
Parser::Parser() : Parser([](auto) { return true; }) {}

Parser::Parser(std::function<bool(const std::string&)> filter)
    : impl_(std::make_unique<ParserImpl>(filter)) {}

Parser::~Parser() = default;

std::vector<wikiopencite::proto::ExtractedCitation> Parser::parse(
    const std::string& text) {

  return this->impl_->parse(text);
}

}  // namespace wikiopencite::citescoop
