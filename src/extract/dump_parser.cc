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
#include "citescoop/proto/revision.pb.h"

namespace wikiopencite::citescoop {
namespace proto = wikiopencite::proto;

DumpParser::DumpParser(std::shared_ptr<wikiopencite::citescoop::Parser> parser)
    : parser_(parser) {}

std::pair<std::unique_ptr<std::vector<proto::Page>>,
          // NOLINTNEXTLINE(whitespace/indent_namespace)
          std::unique_ptr<std::map<uint64_t, proto::Revision>>>
DumpParser::ParseXML(std::istream& stream) {
  stored_pages_ = std::make_unique<std::vector<proto::Page>>();
  stored_revisions_ = std::make_unique<std::map<uint64_t, proto::Revision>>();
  StartParser(stream);
  return std::make_pair(std::move(stored_pages_), std::move(stored_revisions_));
}

void DumpParser::on_start_element(const xmlpp::ustring& name,
                                  const AttributeList&) {
  text_buf_ = "";
  if (name == "page") {
    in_page_ = true;
  } else if (name == "revision") {
    in_revision_ = true;
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
    OnEndPage();
  } else if (name == "revision") {
    OnEndRevision();
  } else if (name == "contributor") {
    in_contributor_ = false;
  } else {
    OnEndField(name);
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

void DumpParser::Store(const std::map<uint64_t, proto::Revision>& revisions,
                       const proto::Page& page) {
  stored_pages_->push_back(page);
  stored_revisions_->insert(revisions.begin(), revisions.end());
}

void DumpParser::StartParser(std::istream& stream) {
  InitializeParser();
  set_substitute_entities(true);
  parse_stream(stream);
}

void DumpParser::InitializeParser() {
  revisions_to_store_ = std::map<uint64_t, proto::Revision>();
  current_page_ = proto::Page();
  current_revision_ = proto::Revision();
  current_page_revisions_ = std::map<uint64_t, proto::Revision>();
  citations_by_revision_ = std::vector<proto::RevisionCitations>();
  ResetState();
}

void DumpParser::CheckExistingCitations(
    proto::RevisionCitations* revision,
    std::map<std::string, proto::Citation>* discovered_citations,
    std::map<uint64_t, int>* ref_count) {

  for (auto& [key, citation] : *discovered_citations) {
    if (revision->citations().contains(key)) {
      revision->mutable_citations()->erase(key);

      // Just make sure that we don't mark it as removed. NOTE: This
      // is a slight technical limitation, if a citation is removed
      // from a article and then re-added, we won't detect that is was
      // re-added and will just show that it continues to be there.
      if (citation.has_revision_removed()) {
        (*ref_count)[citation.revision_removed()]--;
        if (ref_count->at(citation.revision_removed()) <= 0) {
          revisions_to_store_.erase(citation.revision_removed());
        }

        citation.clear_revision_removed();
      }
    } else {
      if (!citation.has_revision_removed()) {
        auto id = revision->revision().revision_id();
        citation.set_revision_removed(id);
        revisions_to_store_.insert({id, current_page_revisions_.at(id)});
        (*ref_count)[id]++;
      }
    }
  }
}

void DumpParser::AddNewCitations(
    wikiopencite::proto::RevisionCitations* citations,
    std::map<std::string, wikiopencite::proto::Citation>* discovered_citations,
    std::map<uint64_t, int>* ref_count) {
  for (const auto& [key, extracted_citation] : citations->citations()) {
    if (!discovered_citations->contains(key)) {
      auto citation = proto::Citation();
      auto id = citations->revision().revision_id();
      citation.set_revision_added(id);

      revisions_to_store_.insert({id, current_page_revisions_.at(id)});
      (*ref_count)[id]++;

      citation.mutable_citation()->CopyFrom(extracted_citation);
      discovered_citations->insert({key, citation});
    }
  }
}

void DumpParser::MakePageCitationList() {
  // Sort revisions by date
  std::ranges::sort(
      citations_by_revision_,
      [](const proto::RevisionCitations& a, const proto::RevisionCitations& b) {
        return a.revision().timestamp().seconds() ==
                       b.revision().timestamp().seconds()
                   ? a.revision().timestamp().nanos() <
                         b.revision().timestamp().nanos()
                   : a.revision().timestamp().seconds() <
                         b.revision().timestamp().seconds();
      });

  auto discovered_citations = std::map<std::string, proto::Citation>();
  auto revisions_ref_count = std::map<uint64_t, int>();

  for (auto& citations : citations_by_revision_) {
    CheckExistingCitations(&citations, &discovered_citations,
                           &revisions_ref_count);
    AddNewCitations(&citations, &discovered_citations, &revisions_ref_count);
  }

  // Copy the complete set of citations into the page.
  for (const auto& [key, citation] : discovered_citations) {
    auto added_citation = current_page_.add_citations();
    added_citation->CopyFrom(citation);
  }
}

void DumpParser::ResetState() {
  in_page_ = false;
  in_revision_ = false;
  in_contributor_ = false;
  should_store_ = false;
}

void DumpParser::OnEndField(const xmlpp::ustring& field_name) {
  if (in_page_ && field_name == "title") {
    current_page_.set_title(text_buf_);
  } else if (in_page_ && !in_revision_ && !in_contributor_ &&
             field_name == "id") {
    current_page_.set_page_id(static_cast<uint64_t>(std::stol(text_buf_)));
  } else if (in_revision_ && !in_contributor_ && field_name == "id") {
    current_revision_.set_revision_id(
        static_cast<uint64_t>(std::stol(text_buf_)));
  } else if (in_revision_ && field_name == "parentid") {
    current_revision_.set_parent_id(
        static_cast<uint64_t>(std::stol(text_buf_)));
  } else if (in_revision_ && field_name == "username") {
    current_revision_.set_user(text_buf_);
  } else if (in_revision_ && field_name == "text") {
    current_citations_ = parser_->Parse(text_buf_);
  } else if (in_revision_ && field_name == "timestamp") {
    auto timestamp = google::protobuf::Timestamp();
    google::protobuf::util::TimeUtil::FromString(text_buf_, &timestamp);
    current_revision_.mutable_timestamp()->CopyFrom(timestamp);
  }
}

void DumpParser::OnEndPage() {
  in_page_ = false;
  MakePageCitationList();

  Store(revisions_to_store_, current_page_);

  // Clear everything up
  current_page_.Clear();
  current_page_revisions_.clear();
  citations_by_revision_.clear();
}

void DumpParser::OnEndRevision() {
  in_revision_ = false;
  current_citations_.mutable_revision()->CopyFrom(current_revision_);
  citations_by_revision_.push_back(current_citations_);
  current_page_revisions_.insert(
      {current_revision_.revision_id(), current_revision_});

  current_revision_.Clear();
}

}  // namespace wikiopencite::citescoop
