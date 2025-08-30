// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cli.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/positional_options.hpp"
#include "boost/program_options/value_semantic.hpp"
#include "boost/program_options/variables_map.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"

#include "citescoop/version.h"
#include "commands/base_command.h"

namespace options = boost::program_options;

namespace wikiopencite::citescoop::cli {
Cli::Cli() : global_options_("Global options") {
  global_options_.add_options()("help", "Show global help message")(
      "version", "Show citescoop version")(
      "command", options::value<std::string>(), "command to execute")(
      "subargs", options::value<std::vector<std::string>>(),
      "Arguments for command");

  positional_options_.add("command", 1).add("subargs", -1);
}

void Cli::Register(std::unique_ptr<BaseCommand> command) {
  spdlog::trace("Registering command {}", command->name());
  this->commands_.insert({command->name(), std::move(command)});
}

int Cli::Run(int argc,
             char* argv[]) {  // NOLINT(modernize-avoid-c-arrays)
  auto global_args = ParseGlobalArgs(argc, argv);

  // Check our --help and --version flags first
  if (!global_args.first["help"].empty()) {
    PrintGlobalHelp();
    return EXIT_SUCCESS;
  }

  if (!global_args.first["version"].empty()) {
    PrintVersion();
    return EXIT_SUCCESS;
  }

  if (global_args.first["command"].empty()) {
    spdlog::critical("No command passed");
    std::cout << "Missing required argument command" << '\n';
    return static_cast<int>(ExitCode::kCliArgsError);
  }

  std::string cmd = global_args.first["command"].as<std::string>();
  if (!commands_.contains(cmd)) {
    spdlog::critical("Command {} not found", cmd);
    return static_cast<int>(ExitCode::kCliArgsError);
  }

  return EXIT_SUCCESS;
}

std::pair<options::variables_map, options::parsed_options> Cli::ParseGlobalArgs(
    int argc, char* argv[]) {  //NOLINT (modernize-avoid-c-arrays)

  const options::parsed_options parsed =
      options::command_line_parser(argc, argv)
          .options(global_options_)
          .positional(positional_options_)
          .allow_unregistered()
          .run();

  options::variables_map cli_variables;
  options::store(parsed, cli_variables);

  return std::pair<options::variables_map, options::parsed_options>(
      cli_variables, parsed);
}

void Cli::PrintVersion() {
  namespace cmake = wikiopencite::citescoop::cmake;
  std::cout << fmt::format("{} v{} ({})", cmake::project_name,
                           cmake::project_version, cmake::git_sha)
            << '\n';
}

void Cli::PrintGlobalHelp() {
  namespace cmake = wikiopencite::citescoop::cmake;
  std::cout << fmt::format("Usage: {} [global options] <command> [<args>]",
                           cmake::project_name)
            << '\n';

  // BUG(computroniks) This will also print out --command and --subargs
  // which may not be intentional.
  std::cout << global_options_;
  std::cout << '\n' << "Available commands: " << '\n';

  for (auto const& [_, command] : commands_) {
    std::cout << fmt::format("{}\t\t{}", command->name(),
                             command->description())
              << '\n';
  }
}

}  // namespace wikiopencite::citescoop::cli
