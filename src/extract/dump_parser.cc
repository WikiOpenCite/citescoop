// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dump_parser.h"

#include <istream>
#include <memory>
#include <vector>

#include "citescoop/proto/page.pb.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

DumpParser::DumpParser(std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : parser_(parser) {}

std::unique_ptr<std::vector<proto::Page>> DumpParser::ParseXML(
    std::istream& stream) {
  std::unique_ptr<std::vector<proto::Page>> pages(new std::vector<proto::Page>);
  char buf[64];
  const size_t buf_size = sizeof(buf) / sizeof(char);

  set_substitute_entities(true);
  do {
    std::memset(buf, 0, buf_size);
    stream.read(buf, buf_size - 1);
    if (stream.gcount()) {
      xmlpp::ustring input(buf, buf + stream.gcount());
      parse_chunk(input);
    }
  } while (stream);

  finish_chunk_parsing();

  return pages;
}

void DumpParser::on_start_document() {
  // std::cout << "on_start_document()" << std::endl;
}

void DumpParser::on_end_document() {
  // std::cout << "on_end_document()" << std::endl;
}

void DumpParser::on_start_element(const xmlpp::ustring& name,
                                  const AttributeList& attributes) {
  // std::cout << "node name=" << name << std::endl;

  // // Print attributes:
  // for (const auto& attr_pair : attributes) {
  //   std::cout << "  Attribute name=" << attr_pair.name << std::endl;
  //   std::cout << "    , value= " << attr_pair.value << std::endl;
  // }
}

void DumpParser::on_end_element(const xmlpp::ustring& /* name */) {
  // std::cout << "on_end_element()" << std::endl;
}

void DumpParser::on_characters(const xmlpp::ustring& text) {
  // std::cout << "on_characters(): " << text << std::endl;
}

void DumpParser::on_comment(const xmlpp::ustring& text) {
  // std::cout << "on_comment(): " << text << std::endl;
}

void DumpParser::on_warning(const xmlpp::ustring& text) {
  // std::cout << "on_warning(): " << text << std::endl;
}

void DumpParser::on_error(const xmlpp::ustring& text) {
  // std::cout << "on_error(): " << text << std::endl;
}

void DumpParser::on_fatal_error(const xmlpp::ustring& text) {
  // std::cout << "on_fatal_error(): " << text << std::endl;
}

}  // namespace wikiopencite::citescoop
