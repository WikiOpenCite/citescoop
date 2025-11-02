// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dump_parser.h"

#include <istream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"

#include "citescoop/extract.h"
#include "citescoop/proto/page.pb.h"
#include "citescoop/proto/revision_map.pb.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

DumpParser::DumpParser(std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : parser_(parser) {}

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          // NOLINTNEXTLINE(whitespace/indent_namespace)
          std::unique_ptr<proto::RevisionMap>>
DumpParser::ParseXML(std::istream& stream) {
  pages_ =
      std::unique_ptr<std::vector<proto::Page>>(new std::vector<proto::Page>);
  revisions_ = std::unique_ptr<proto::RevisionMap>(new proto::RevisionMap);
  page_revisions_ = std::map<int64_t, proto::Revision>();

  set_substitute_entities(true);
  parse_stream(stream);

  return std::make_pair(std::move(pages_), std::move(revisions_));
}

void DumpParser::on_start_document() {
  // Reset all our flags to make sure
  in_page_ = false;
  in_revision_ = false;
  in_contributor_ = false;
  should_store_ = false;
}

void DumpParser::on_end_document() {
  // std::cout << "on_end_document()" << std::endl;
}

void DumpParser::on_start_element(const xmlpp::ustring& name,
                                  const AttributeList&) {
  text_buf_ = "";

  if (name == "page") {
    in_page_ = true;
    current_page_ = proto::Page();
    all_page_citations_ = std::vector<proto::RevisionCitations>();
    page_revisions_.clear();
  } else if (name == "revision") {
    in_revision_ = true;
    current_revision_ = proto::Revision();
  } else if (name == "contributor") {
    in_contributor_ = true;
  } else if ((in_page_ && name == "title") || (in_page_ && name == "id") ||
             (in_revision_ && name == "id") ||
             (in_revision_ && name == "parentid") ||
             (in_revision_ && name == "username") ||
             (in_revision_ && name == "text") ||
             (in_revision_ && name == "timestamp")) {
    should_store_ = true;
  }
}

void DumpParser::on_end_element(const xmlpp::ustring& name) {
  if (name == "page") {
    in_page_ = false;
    MakePageCitationList();
    pages_->push_back(current_page_);

  } else if (in_page_ && name == "title") {
    current_page_.set_title(text_buf_);
  } else if (in_page_ && !in_revision_ && !in_contributor_ && name == "id") {
    current_page_.set_page_id(std::stol(text_buf_));
  } else if (name == "revision") {
    in_revision_ = false;

    revision_citations_.mutable_revision()->CopyFrom(current_revision_);
    all_page_citations_.push_back(revision_citations_);

    page_revisions_.insert(
        {current_revision_.revision_id(), current_revision_});
  } else if (in_revision_ && !in_contributor_ && name == "id") {
    current_revision_.set_revision_id(std::stol(text_buf_));
  } else if (in_revision_ && name == "parentid") {
    current_revision_.set_parent_id(std::stol(text_buf_));
  } else if (in_revision_ && name == "username") {
    current_revision_.set_user(text_buf_);
  } else if (in_revision_ && name == "text") {
    revision_citations_ = parser_->Parse(text_buf_);
  } else if (in_revision_ && name == "timestamp") {
    auto timestamp = google::protobuf::Timestamp();
    google::protobuf::util::TimeUtil::FromString(text_buf_, &timestamp);
    current_revision_.mutable_timestamp()->CopyFrom(timestamp);
  } else if (name == "contributor") {
    in_contributor_ = false;
  }

  if (should_store_)
    should_store_ = false;
}

void DumpParser::on_characters(const xmlpp::ustring& characters) {
  if (should_store_)
    text_buf_ += characters;
}

void DumpParser::on_warning(const xmlpp::ustring& text) {
  throw DumpParseException(text);
}

void DumpParser::on_error(const xmlpp::ustring& text) {
  throw DumpParseException(text);
}

void DumpParser::on_fatal_error(const xmlpp::ustring& text) {
  throw DumpParseException(text);
}

void DumpParser::MakePageCitationList() {
  // First sort revisions by date
  std::ranges::sort(all_page_citations_, [](const proto::RevisionCitations& a,
                                            const proto::RevisionCitations& b) {
    return a.revision().timestamp().seconds() ==
                   b.revision().timestamp().seconds()
               ? a.revision().timestamp().nanos() <
                     b.revision().timestamp().nanos()
               : a.revision().timestamp().seconds() <
                     b.revision().timestamp().seconds();
  });

  auto deduplicated_citations = std::map<std::string, proto::Citation>();

  // Then for each revision go through each citation
  for (auto& revision : all_page_citations_) {
    for (auto& [key, citation] : deduplicated_citations) {
      if (revision.citations().contains(key)) {
        revision.mutable_citations()->erase(key);

        // Just make sure that we don't mark it as removed. NOTE: This
        // is a slight technical limitation, if a citation is removed
        // from a article and then re-added, we won't detect that is was
        // re-added and will just show that it continues to be there.
        citation.clear_revision_removed();
      } else {
        if (!citation.has_revision_removed()) {
          auto id = revision.revision().revision_id();
          citation.set_revision_removed(id);
          revisions_->mutable_revisions()->insert({id, page_revisions_.at(id)});
        }
      }
    }
    for (const auto& [key, extracted_citation] : revision.citations()) {
      if (!deduplicated_citations.contains(key)) {
        auto citation = proto::Citation();
        auto id = revision.revision().revision_id();
        citation.set_revision_added(id);
        revisions_->mutable_revisions()->insert({id, page_revisions_.at(id)});
        citation.mutable_citation()->CopyFrom(extracted_citation);
        deduplicated_citations.insert({key, citation});
      }
    }
  }

  for (const auto& [key, citation] : deduplicated_citations) {
    auto added_citation = current_page_.add_citations();
    added_citation->CopyFrom(citation);
  }
}

}  // namespace wikiopencite::citescoop
