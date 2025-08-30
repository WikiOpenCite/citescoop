// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ios>
#include <sstream>
#include <string>

#include "spdlog/common.h"
#include "spdlog/spdlog.h"

#include "cli.h"

using wikiopencite::citescoop::cli::Cli;

namespace {
void SetDebug() {
  spdlog::set_level(spdlog::level::off);

  auto* debug_env = std::getenv("DEBUG");
  if (debug_env != nullptr) {
    auto debug_str = std::string(debug_env);
    std::ranges::transform(debug_str, debug_str.begin(),
                           [](unsigned char chr) { return std::tolower(chr); });
    std::istringstream iss(debug_env);
    bool debug;
    iss >> std::boolalpha >> debug;

    if (debug) {
      spdlog::set_level(spdlog::level::trace);
    }
  }
}
}  // namespace

auto main(int argc, char** argv) -> int {
  SetDebug();

  Cli cli = Cli();

  return cli.Run(argc, argv);
}
