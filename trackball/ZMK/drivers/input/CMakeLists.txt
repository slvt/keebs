# Copyright (c) 2025 George Norton
# SPDX-License-Identifier: MIT

zephyr_library_amend()

zephyr_library_sources_ifdef(CONFIG_INPUT_PIXART_PMW33XX input_pmw33xx.c)
