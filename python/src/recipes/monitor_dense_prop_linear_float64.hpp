/*
 * Copyright (c) 2019-2020 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "string"

#include "reelay/parser/ptl.hpp"
#include "reelay/settings.hpp"
#include "reelay/pydict.hpp"

#include "reelay/targets/pybind11/formatter_dense_timed.hpp"

namespace reelay {

struct monitor_dense_prop_linear_float64 {

  using time_t = double;
  using value_t = bool;
  using input_t = pybind11::object;
  using output_t = pybind11::list;

  std::string name;

  using factory = dense_timed_setting::factory<input_t, time_t, 1>;

  using network_t = typename factory::network_t;
  using network_ptr_t = typename factory::network_ptr_t;

  using formatter_t = formatter_dense_timed<time_t>;

  network_ptr_t network;
  formatter_t formatter;

  explicit monitor_dense_prop_linear_float64(const std::string &pattern,
                                             const std::string t_str = "time",
                                             const std::string y_str = "value")
      : formatter(formatter_t(t_str, y_str)) {
    auto parser = ptl_parser<factory>();
    this->network = parser.parse(pattern);
  }

  output_t update(const input_t &args) {
    this->network->update(args);
    auto result = network->output();
    return formatter.format(result, network->previous, network->current);
  }

  std::pair<time_t, time_t> now() { return network->now(); }
};

} // namespace reelay
