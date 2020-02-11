/*
 * Copyright (c) 2019-2020 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "string"
#include "reelay/parser/ptl_parser.hpp"
#include "reelay/settings.hpp"

namespace reelay {

struct discrete_timed_past_mtl_monitor {

  using time_t  = int64_t;
  using input_t = std::unordered_map<std::string, time_t>;
  using factory = discrete_timed_setting::factory<input_t, time_t>;

  using network_t = typename factory::network_t;
  using network_ptr_t = typename factory::network_ptr_t;

  network_ptr_t network;

  explicit discrete_timed_past_mtl_monitor(const std::string &pattern) {
    auto parser = ptl_parser<factory>();
    this->network = parser.parse(pattern);
  }

  bool update(const input_t &args) {
    this->network->update(args);
    return this->network->output();
  }
  
  time_t now() { return network->now(); }
};

} // namespace reelay