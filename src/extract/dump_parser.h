// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_EXTRACT_DUMP_PARSER_H_
#define SRC_EXTRACT_DUMP_PARSER_H_

#include <cstdint>
#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "citescoop/parser.h"
#include "citescoop/proto/citation.pb.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision.pb.h"
#include "citescoop/proto/revision_citations.pb.h"
#include "libxml++/parsers/saxparser.h"
#include "libxml++/ustring.h"

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
  // void on_start_document() override;
  // void on_end_document() override;
  void on_start_element(const xmlpp::ustring& name,
                        const AttributeList& properties) override;
  void on_end_element(const xmlpp::ustring& name) override;
  void on_characters(const xmlpp::ustring& characters) override;
  void on_warning(const xmlpp::ustring& text) override;
  void on_error(const xmlpp::ustring& text) override;
  void on_fatal_error(const xmlpp::ustring& text) override;

  /// @brief Store a page and it's referenced revisions.
  /// Once the parser calls this method for a given page, it has
  /// finished with the page so you may do with it as you wish.
  /// @param revisions Revisions to store.
  /// @param page Page to store.
  virtual void Store(
      const std::map<uint64_t, wikiopencite::proto::Revision>& revisions,
      const wikiopencite::proto::Page& page);

  /// @brief Start parsing the input stream.
  /// @param stream Input stream to parse.
  void StartParser(std::istream& stream);

 private:
  std::shared_ptr<wikiopencite::citescoop::Parser> parser_;

  // Flags for where we are in the XML document
  bool in_page_;
  bool in_revision_;
  bool in_contributor_;
  bool should_store_;
  bool store_revision_;

  std::string text_buf_;
  std::vector<wikiopencite::proto::RevisionCitations> citations_by_revision_;
  wikiopencite::proto::RevisionCitations current_citations_;
  wikiopencite::proto::Page current_page_;
  wikiopencite::proto::Revision current_revision_;
  std::map<uint64_t, wikiopencite::proto::Revision> current_page_revisions_;
  std::map<uint64_t, wikiopencite::proto::Revision> revisions_to_store_;

  std::unique_ptr<std::vector<wikiopencite::proto::Page>> stored_pages_;
  std::unique_ptr<std::map<uint64_t, wikiopencite::proto::Revision>>
      stored_revisions_;

  /// @brief Complete the pages citations.
  ///
  /// Will deduplicate the citations and make sure only the first and
  /// last revision is referenced by the citation.
  void MakePageCitationList();

  /// @brief Reset the parser state.
  void ResetState();

  /// @brief Handle when a field ends.
  /// Handles fields such as id, text and so on.
  /// @param field_name Name of the field that has ended.
  void OnEndField(const xmlpp::ustring& field_name);

  /// @brief Handle the end of a page.
  /// Will assemble the deduplicated page citations list, store the
  /// current page and then clear any datastructures used in page
  /// processing ready for the next page.
  void OnEndPage();

  /// @brief Handle the end of a revision.
  /// Adds the current revision and it's citations to the required
  /// datastructures and clears it ready for the next one.
  void OnEndRevision();

  /// @brief Initialize the required datastructures for the parser.
  void InitializeParser();

  /// @brief Check if any citations already discovered exist in this
  /// revision.
  /// Any citations that are not found in this revision and that have
  /// not already been marked as removed will be marked as removed in
  /// this revision. Any citations that have been previously marked as
  /// removed but have been re-added in this revision will be marked as
  /// present again. Note: In this case, the revision_added field will
  /// still show the revision in which the citation was originally
  /// added, the fact it was removed and re-added will not be stored and
  /// this information is lost.
  ///
  /// @param citations Citations in this revision.
  /// @param discovered_citations Map of citations previously discovered
  /// for this page.
  /// @param ref_count Map of the number of references for each
  /// revision. Used to make sure only revisions with references are
  /// stored.
  void CheckExistingCitations(
      wikiopencite::proto::RevisionCitations* revision,
      std::map<std::string, wikiopencite::proto::Citation>*
          discovered_citations,
      std::map<uint64_t, int>* ref_count);

  /// @brief Add any citations not already discovered.
  /// For any citations that do not already exist in
  /// discovered_citations, this add them setting the revision_added
  /// field to the current revision ID.
  /// @param citations Citations in this revision.
  /// @param discovered_citations Map of citations already found for
  /// this page.
  /// @param ref_count Map of the number of references for each
  /// revision. Used to make sure only revisions with references are
  /// stored.
  void AddNewCitations(wikiopencite::proto::RevisionCitations* citations,
                       std::map<std::string, wikiopencite::proto::Citation>*
                           discovered_citations,
                       std::map<uint64_t, int>* ref_count);
};
}  // namespace wikiopencite::citescoop

#endif  // SRC_EXTRACT_DUMP_PARSER_H_
