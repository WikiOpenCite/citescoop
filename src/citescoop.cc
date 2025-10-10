// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#include "citescoop.h"

#include <string>

#include "fmt/core.h"

#include "citescoop/citescoop.h"

exported_class::exported_class() : m_name{fmt::format("{}", "citescoop")} {}

auto exported_class::name() const -> char const* {
  return m_name.c_str();
}
