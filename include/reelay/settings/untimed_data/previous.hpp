/*
 * Copyright (c) 2019-2020 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "vector"

#include "reelay/networks/basic_structure.hpp"

namespace reelay {
namespace untimed_data_setting {

template <typename X> 
struct previous : public untimed_state<X, data_set_t> {
  using input_t = X;
  using output_t = data_set_t;

  using node_t = untimed_node<output_t>;
  using node_ptr_t = std::shared_ptr<node_t>;
  
  data_mgr_t manager;

  node_ptr_t first;

  data_set_t prev_value;
  data_set_t value;
  
  explicit previous(const data_mgr_t &mgr, const std::vector<node_ptr_t> &args)
      : manager(mgr), first(args[0]) {
        prev_value = mgr->zero();
        value = mgr->zero();
      }

  explicit previous(const kwargs &kw)
      : previous(reelay::any_cast<data_mgr_t>(kw.at("manager")),
                 reelay::any_cast<std::vector<node_ptr_t>>(kw.at("args"))) {}

  void update(const input_t &) override {
    prev_value = value;
    value = first->output();
  }

  output_t output() override { return prev_value; }
};

} // namespace untimed_data_setting
} // namespace reelay