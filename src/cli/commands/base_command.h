// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_CLI_COMMANDS_BASE_COMMAND_H_
#define SRC_CLI_COMMANDS_BASE_COMMAND_H_

#include <string>

#include "spdlog/common.h"

namespace wikiopencite::citescoop::cli {
struct GlobalOptions {
  spdlog::level::level_enum log_level;
};

// Base type for CLI commands
class BaseCommand {
 public:
  explicit BaseCommand(std::string name, std::string description);
  virtual ~BaseCommand() = default;

  virtual int Run(int argc, std::array<char*, 0> argv,
                  struct GlobalOptions globals) = 0;

  std::string description() { return description_; }

  std::string name() { return name_; }

 private:
  std::string name_;
  std::string description_;
};

}  // namespace wikiopencite::citescoop::cli

#endif  // SRC_CLI_COMMANDS_BASE_COMMAND_H_
