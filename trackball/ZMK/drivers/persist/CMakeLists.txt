# SPDX-License-Identifier: MIT

target_sources(app PRIVATE trackball_persist.c)

# The commit the firmware is built from, used to tell builds apart. Without git
# the source falls back to the compile time stamp.
execute_process(
  COMMAND git -c safe.directory=* -C ${CMAKE_CURRENT_SOURCE_DIR} rev-parse --short=12 HEAD
  OUTPUT_VARIABLE TRACKBALL_GIT_REV
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_QUIET
  RESULT_VARIABLE TRACKBALL_GIT_RESULT
)
if(TRACKBALL_GIT_RESULT EQUAL 0 AND TRACKBALL_GIT_REV)
  target_compile_definitions(app PRIVATE TRACKBALL_BUILD_STAMP="${TRACKBALL_GIT_REV}")
endif()
