// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_DUMP_PARSER_H_
#define SRC_EXTRACT_DUMP_PARSER_H_

#include <iostream>
#include <istream>
#include <memory>
#include <vector>

#include "libxml++/libxml++.h"

#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"

namespace wikiopencite::citescoop {
class DumpParser : public xmlpp::SaxParser {
 public:
  explicit DumpParser(std::shared_ptr<wikiopencite::citescoop::Parser> parser);
  std::unique_ptr<std::vector<wikiopencite::proto::Page>> ParseXML(
      std::istream& stream);

 protected:
  // SAX parser overrides
  void on_start_document() override;
  void on_end_document() override;
  void on_start_element(const xmlpp::ustring& name,
                        const AttributeList& properties) override;
  void on_end_element(const xmlpp::ustring& name) override;
  void on_characters(const xmlpp::ustring& characters) override;
  void on_comment(const xmlpp::ustring& text) override;
  void on_warning(const xmlpp::ustring& text) override;
  void on_error(const xmlpp::ustring& text) override;
  void on_fatal_error(const xmlpp::ustring& text) override;

  std::shared_ptr<wikiopencite::citescoop::Parser> parser_;
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_DUMP_PARSER_H_
