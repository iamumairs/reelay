/*
 * Copyright (c) 2019-2020 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "functional"
#include "memory"
#include "vector"

#include "reelay/common.hpp"
#include "reelay/datafield.hpp"
#include "reelay/intervals.hpp"
#include "reelay/networks/basic_structure.hpp"
#include "reelay/unordered_data.hpp"

namespace reelay {
namespace untimed_data_setting {

template <typename X, typename K = std::string>
struct atomic_any : public untimed_state<X, data_set_t> {
  using key_t = K;
  using input_t = X;
  using output_t = data_set_t;

  using node_t = untimed_node<output_t>;
  using node_ptr_t = std::shared_ptr<node_t>;

  data_mgr_t manager;
  data_set_t value;

  key_t key;

  explicit atomic_any(const data_mgr_t &mgr, const key_t &key)
      : manager(mgr), value(mgr->one()), key(key) {}

  explicit atomic_any(const kwargs &kw)
      : atomic_any(reelay::any_cast<data_mgr_t>(kw.at("manager")),
                   reelay::any_cast<key_t>(kw.at("key"))) {}

  void update(const input_t &args) override {
    bool key_exists = datafield<input_t>::contains(args, key);
    if (key_exists) {
      value = manager->one();
    } else {
      value = manager->zero();
    }
  }

  output_t output() override { return value; }
};

} // namespace untimed_data_setting
} // namespace reelay