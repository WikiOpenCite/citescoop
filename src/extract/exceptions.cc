// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdexcept>
#include <string>

#include "citescoop/extract.h"

namespace wikiopencite::citescoop {
DumpParseException::DumpParseException(const std::string& message)
    : std::runtime_error("Dump parse failure: " + message) {}

}  // namespace wikiopencite::citescoop
