# SPDX-License-Identifier: MIT
find_package(RE2C 3 REQUIRED)
RE2C_TARGET(NAME lexers_ut_group_handler INPUT ${PROJECT_SOURCE_DIR}/src/lexers.re OUTPUT lexers.c OPTIONS ${RE2C_OPTIONS} -W DEPENDS ${PROJECT_SOURCE_DIR}/inc/lexers.h)

create_unit_test(
  NAME
    utest-crinit-cfg-group-handler
  SOURCES
    utest-crinit-cfg-group-handler.c
    case-empty-input.c
    case-negative-input.c
    case-null-input.c
    case-success.c
    lexers.c
    ${PROJECT_SOURCE_DIR}/src/logio.c
    ${PROJECT_SOURCE_DIR}/src/confconv.c
    ${PROJECT_SOURCE_DIR}/src/confhdl.c
    ${PROJECT_SOURCE_DIR}/src/confparse.c
  LIBRARIES
    libmockfunctions
    inih-local
  WRAPS
    -Wl,--wrap=getgrnam_r
    -Wl,--wrap=getgrgid_r
)
