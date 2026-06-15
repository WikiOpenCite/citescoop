// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "citescoop/openalex.h"
#include "citescoop/proto/language.pb.h"
#include "citescoop/proto/openalex/work.pb.h"
#include "google/protobuf/util/time_util.h"

#include "util.h"  // NOLINT(misc-include-cleaner)

#define TEST_NAME_PREFIX "[openalex/SnapshotProcessor] "

namespace proto = wikiopencite::proto;
namespace openalex = wikiopencite::citescoop::openalex;

SCENARIO(TEST_NAME_PREFIX "all fields are extracted correctly from the record",
         "[openalex][openalex / SnapshotProcessor]") {
  GIVEN("a file containing a single record") {
    auto processor = openalex::SnapshotProcessor();

    std::ifstream file(
        FILE("data/single-line.jsonl"));  // NOLINT(misc-include-cleaner)
    REQUIRE(file.is_open());

    WHEN("the file is parsed") {
      auto [authors, institutions, works] =
          processor.ProcessWorksSnapshot(file);

      THEN("a single author was extracted") {
        REQUIRE(authors.size() == 1);
      }

      WHEN("the author is retrieved") {
        auto author = authors.at(0);

        THEN("the author has the correct values") {
          REQUIRE(author.openalex_id() == "A123456789");
          REQUIRE(author.name() == "John Doe");
          REQUIRE(author.orcid() == "0000-0001-2345-6789");
        }
      }

      THEN("a single institution was extracted") {
        REQUIRE(institutions.size() == 1);
      }

      WHEN("the institution is retrieved") {
        auto institution = institutions.at(0);
        THEN("the institution has the correct values") {
          REQUIRE(institution.openalex_id() == "I123456789");
          REQUIRE(institution.name() == "Example University");
          REQUIRE(institution.ror() == "https://ror.org/012345678");
        }
      }

      THEN("a single work was extracted") {
        REQUIRE(works.size() == 1);
      }

      WHEN("the work is retrieved") {
        auto work = works.at(0);
        THEN("the IDs match") {
          REQUIRE(work.openalex_id() == "W1234567890");
          REQUIRE(work.ids().doi() == "10.1234/example.doi");
          REQUIRE(work.ids().pmid() == 12345678);
        }

        THEN("the publication date matches") {
          auto timestamp = google::protobuf::Timestamp();
          google::protobuf::util::TimeUtil::FromString("2023-01-01",
                                                       &timestamp);
          REQUIRE(work.publication_date().seconds() == timestamp.seconds());
          REQUIRE(work.publication_date().nanos() == timestamp.nanos());
        }

        THEN("the authors and institutions match") {
          REQUIRE(work.author_ids_size() == 1);
          REQUIRE(work.author_ids(0) == "A123456789");
          REQUIRE(work.institution_ids_size() == 1);
          REQUIRE(work.institution_ids(0) == "I123456789");
        }

        THEN("the open access fields match") {
          REQUIRE(work.oa_status() ==
                  proto::openalex::Work_OACategory_OA_CATEGORY_GOLD);
          REQUIRE(work.oa_url() == "https://example.com/oa");
        }

        THEN("the language is detected correctly") {
          REQUIRE(work.language() == proto::Language::LANGUAGE_ENGLISH);
        }

        THEN("the title is detected correctly") {
          REQUIRE(work.title() == "Sample Work Title");
        }
      }
    }
  }
}

SCENARIO(TEST_NAME_PREFIX
         "missing and null values in OpenAlex dump are handled gracefully",
         "[openalex][openalex / SnapshotProcessor]") {

  GIVEN("an OpenAlex dump with missing / null fields, one missing per line") {
    auto processor = openalex::SnapshotProcessor();

    std::ifstream file(FILE("data/missing-fields.jsonl"));
    REQUIRE(file.is_open());

    WHEN("the file is parsed") {
      auto [authors, institutions, works] =
          processor.ProcessWorksSnapshot(file);

      THEN("the correct number of elements has been extracted") {
        REQUIRE(authors.size() == 1);
        REQUIRE(institutions.size() == 1);
        REQUIRE(works.size() == 16);
      }
    }
  }
}

SCENARIO(TEST_NAME_PREFIX
         "additional whitespace in OpenAlex dumps does not impair processing",
         "[openalex][openalex / SnapshotProcessor]") {
  GIVEN("an OpenAlex dump with excessive whitespace") {
    auto processor = openalex::SnapshotProcessor();

    std::ifstream file(FILE("data/extra-whitespace.jsonl"));
    REQUIRE(file.is_open());

    WHEN("the file is parsed") {
      auto [authors, institutions, works] =
          processor.ProcessWorksSnapshot(file);
      THEN("the correct number of entries are extracted") {
        REQUIRE(authors.size() == 1);
        REQUIRE(institutions.size() == 1);
        REQUIRE(works.size() == 16);
      }
    }
  }
}

SCENARIO(TEST_NAME_PREFIX
         "open access categories in OpenAlex dumps are correctly recognized",
         "[openalex][openalex / SnapshotProcessor]") {
  GIVEN("an OpenAlex dump file with all categories present") {
    auto processor = openalex::SnapshotProcessor();

    std::ifstream file(
        FILE("data/multi-oa.jsonl"));  // NOLINT(misc-include-cleaner)
    REQUIRE(file.is_open());

    auto [authors, institutions, works] = processor.ProcessWorksSnapshot(file);

    THEN("diamond is correctly parsed") {
      REQUIRE(works.at(0).oa_status() ==
              proto::openalex::Work_OACategory_OA_CATEGORY_DIAMOND);
    }

    THEN("gold is correctly parsed") {
      REQUIRE(works.at(1).oa_status() ==
              proto::openalex::Work_OACategory_OA_CATEGORY_GOLD);
    }

    THEN("green is correctly parsed") {
      REQUIRE(works.at(2).oa_status() ==
              proto::openalex::Work_OACategory_OA_CATEGORY_GREEN);
    }

    THEN("hybrid is correctly parsed") {
      REQUIRE(works.at(3).oa_status() ==
              proto::openalex::Work_OACategory_OA_CATEGORY_HYBRID);
    }

    THEN("bronze is correctly parsed") {
      REQUIRE(works.at(4).oa_status() ==
              proto::openalex::Work_OACategory_OA_CATEGORY_BRONZE);
    }

    THEN("closed is correctly parsed") {
      REQUIRE(works.at(5).oa_status() ==
              proto::openalex::Work_OACategory_OA_CATEGORY_CLOSED);
    }

    THEN("unknown values are marked unspecified") {
      REQUIRE(works.at(6).oa_status() ==
              proto::openalex::Work_OACategory_OA_CATEGORY_UNSPECIFIED);
    }
  }
}

SCENARIO(TEST_NAME_PREFIX "unrecognized languages are handled gracefully",
         "[openalex][openalex / SnapshotProcessor]") {
  GIVEN("an OpenAlex dump file with a work in an unrecognized language") {
    auto processor = openalex::SnapshotProcessor();

    std::ifstream file(
        FILE("data/invalid-lang.jsonl"));  // NOLINT(misc-include-cleaner)
    REQUIRE(file.is_open());

    WHEN("file is parsed") {
      auto [authors, institutions, works] =
          processor.ProcessWorksSnapshot(file);

      THEN("language is marked as unspecified") {
        REQUIRE(works.at(0).language() ==
                proto::Language::LANGUAGE_UNSPECIFIED);
      }
    }
  }
}
