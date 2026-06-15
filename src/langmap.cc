// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "langmap.h"

#include <string>

#include "citescoop/languages.h"
#include "citescoop/proto/language.pb.h"

namespace wikiopencite::citescoop {
proto::Language ISOToLanguage(const std::string& iso_code) {
  const LanguageCode* result =
      Languages::lookup(iso_code.c_str(), iso_code.length());
  if (result) {
    return static_cast<proto::Language>(result->lang_value);
  }
  return proto::Language::LANGUAGE_UNSPECIFIED;
}
}  // namespace wikiopencite::citescoop
