# Copyright (c) 2024 tinyVision.ai Inc.
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

#target_sources(${ZEPHYR_CURRENT_LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/event_logger.c)
#target_sources(${ZEPHYR_CURRENT_LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/GlobalLogger_printer.c)
target_include_directories(${ZEPHYR_CURRENT_LIBRARY} PRIVATE .)
zephyr_library_sources(event_logger_shell.c gen/GlobalLogger_printer.c)
