/*
 * Copyright (c) 2019 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "vector"

#include "reelay/common.hpp"
#include "reelay/intervals.hpp"
#include "reelay/networks/basic_structure.hpp"

namespace reelay {
namespace discrete_timed_setting {

template <typename X, typename T>
struct past_sometime_bounded_half : public discrete_timed_state<X, bool, T> {
  using time_t = T;
  using input_t = X;
  using output_t = bool;

  using node_t = discrete_timed_node<output_t, time_t>;
  using state_t = discrete_timed_state<input_t, output_t, time_t>;

  using node_ptr_t = std::shared_ptr<node_t>;
  using state_ptr_t = std::shared_ptr<state_t>;

  using interval = reelay::interval<time_t>;
  using interval_set = reelay::interval_set<time_t>;

  interval_set value = interval_set();

  node_ptr_t first;
  time_t lbound;

  past_sometime_bounded_half(const std::vector<node_ptr_t> &args, time_t l=0)
      : first(args[0]), lbound(l) {}

  explicit past_sometime_bounded_half(const kwargs &kw)
      : past_sometime_bounded_half(
            std::any_cast<std::vector<node_ptr_t>>(kw.at("args")),
            std::any_cast<time_t>(kw.at("lbound"))) {}

  void update(const input_t&, time_t now) {
    if (first->output(now)) {
      value = value.add(
          interval::closed(now + lbound, std::numeric_limits<time_t>::max()));
      value = value - interval_set(interval::right_open(0, now));
    }
  }

  output_t output(time_t now) { return boost::icl::contains(value, now); }
};

}  // namespace discrete_timed_setting
}  // namespace reelay