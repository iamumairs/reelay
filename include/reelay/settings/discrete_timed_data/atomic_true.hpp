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
namespace discrete_timed_data_setting {

template <typename X, typename T, typename K = std::string>
struct atomic_true : public discrete_timed_state<X, data_set_t, T> {
  using key_t = K;
  using time_t = T;
  using input_t = X;
  using value_t = data_set_t;
  using output_t = data_set_t;

  data_mgr_t manager;
  data_set_t value;

  key_t key;

  explicit atomic_true(const data_mgr_t &mgr, const key_t &key)
      : manager(mgr), value(mgr->zero()), key(key) {}

  explicit atomic_true(const kwargs &kw)
      : atomic_true(reelay::any_cast<data_mgr_t>(kw.at("manager")),
                     reelay::any_cast<key_t>(kw.at("key"))) {}

  void update(const input_t &args, time_t now) override {

    bool new_data;

    try {
      new_data = datafield<input_t>::as_bool(args, key);
    } catch (const std::out_of_range &e) {
      return; // Do nothing if the key does not exist - existing value persists
    }

    if (new_data) {
      value = manager->one();
    } else {
      value = manager->zero();
    }
  }

  output_t output(time_t) override { return value; }
};

} // namespace untimed_setting
} // namespace reelay