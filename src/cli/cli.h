// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SRC_CLI_CLI_H_
#define SRC_CLI_CLI_H_

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"

#include "commands/base_command.h"

namespace wikiopencite::citescoop::cli {

enum class ExitCode : std::uint8_t {
  kCliArgsError = 3,
};

class Cli {
 public:
  Cli();

  // Registers a command within the CLI.
  //
  // @param command Command to register.
  void Register(std::unique_ptr<BaseCommand> command);

  // Run the specified command if it exists. If the command specified
  // does not exist, an exception will be thrown.
  //
  // @param argc Number of command line arguments in array
  // @param argv Array of command line argument values
  int Run(int argc, char* argv[]);  //NOLINT (modernize-avoid-c-arrays)

 private:
  // Parse the global arguments
  //
  // @param argc Number of command line arguments in array
  // @param argv Array of command line arguments
  // @return Boost variables map and parsed options. Parsed options
  // includes any unrecognized options for parsing by subcommands.
  std::pair<boost::program_options::variables_map,
            boost::program_options::parsed_options>
  ParseGlobalArgs(int argc, char* argv[]);  // NOLINT(modernize-avoid-c-arrays)

  // Print the program version
  static void PrintVersion();

  // Print the global help
  void PrintGlobalHelp();

  // Mapping of command name to command
  std::map<std::string, std::unique_ptr<BaseCommand>> commands_;

  boost::program_options::options_description global_options_;
  boost::program_options::positional_options_description positional_options_;
};
}  // namespace wikiopencite::citescoop::cli

#endif  // SRC_CLI_CLI_H_
