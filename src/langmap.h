// SPDX-FileCopyrightText: 2026 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_LANGMAP_H_
#define SRC_LANGMAP_H_

#include <string>

#include "citescoop/proto/language.pb.h"

namespace wikiopencite::citescoop {

/// @brief Converts an ISO 639-1 language code to a proto::Language enum value
/// @param iso_code The ISO 639-1 language code
/// @return The corresponding proto::Language enum value
wikiopencite::proto::Language ISOToLanguage(const std::string& iso_code);

}  // namespace wikiopencite::citescoop

#endif  // SRC_LANGMAP_H_
