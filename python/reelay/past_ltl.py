#
# Copyright (c) 2019 Dogan Ulus
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
import reelay.recipes

class monitor(object):
	def __new__(cls, pattern:str):
		return reelay.recipes.past_ltl_monitor(pattern)


