// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_DUMP_PARSER_H_
#define SRC_EXTRACT_DUMP_PARSER_H_

#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "libxml++/libxml++.h"

#include "citescoop/parser.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"

namespace wikiopencite::citescoop {

/// @brief MediaWiki XML dump parser.
class DumpParser : public xmlpp::SaxParser {
 public:
  /// @brief Construct a new dumps parser.
  /// @param parser The citation parser to use.
  explicit DumpParser(std::shared_ptr<wikiopencite::citescoop::Parser> parser);

  /// @brief Parse the dump XML.
  /// @param stream An input stream of plain XML. NOTE: if you are
  /// dealing with a compressed dump, this must have already been
  /// decompressed by this point.
  /// @return Pages and referenced revisions.
  std::pair<std::unique_ptr<std::vector<wikiopencite::proto::Page>>,
            std::unique_ptr<std::map<uint64_t, proto::Revision>>>
  ParseXML(std::istream& stream);

 protected:
  // SAX parser overrides
  void on_start_document() override;
  void on_end_document() override;
  void on_start_element(const xmlpp::ustring& name,
                        const AttributeList& properties) override;
  void on_end_element(const xmlpp::ustring& name) override;
  void on_characters(const xmlpp::ustring& characters) override;
  void on_warning(const xmlpp::ustring& text) override;
  void on_error(const xmlpp::ustring& text) override;
  void on_fatal_error(const xmlpp::ustring& text) override;

 private:
  std::shared_ptr<wikiopencite::citescoop::Parser> parser_;

  // Flags for where we are in the XML document
  bool in_page_;
  bool in_revision_;
  bool in_contributor_;
  bool should_store_;
  bool store_revision_;

  std::string text_buf_;
  std::vector<wikiopencite::proto::RevisionCitations> all_page_citations_;
  wikiopencite::proto::RevisionCitations revision_citations_;
  wikiopencite::proto::Page current_page_;
  wikiopencite::proto::Revision current_revision_;
  std::map<int64_t, wikiopencite::proto::Revision> page_revisions_;

  std::unique_ptr<std::vector<wikiopencite::proto::Page>> pages_;
  std::unique_ptr<std::map<uint64_t, wikiopencite::proto::Revision>> revisions_;

  /// @brief Complete the pages citations.
  ///
  /// Will deduplicate the citations and make sure only the first and
  /// last revision is referenced by the citation.
  void MakePageCitationList();
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_DUMP_PARSER_H_
