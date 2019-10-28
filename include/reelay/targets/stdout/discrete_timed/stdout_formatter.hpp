/*
 * Copyright (c) 2019 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "reelay/settings.hpp"

namespace reelay {
namespace discrete_timed_setting {

template <typename X, typename T>
struct stdout_formatter : public discrete_timed_state<X, std::string, T> {

  virtual std::string header();
  virtual std::string output();
};
} // namespace discrete_timed_setting
} // namespace reelay