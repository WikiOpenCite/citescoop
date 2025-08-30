// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "base_command.h"

#include <string>
#include <utility>

namespace wikiopencite::citescoop::cli {
BaseCommand::BaseCommand(std::string name, std::string description) {
  name_ = std::move(name);
  description_ = std::move(description);
}
}  // namespace wikiopencite::citescoop::cli
