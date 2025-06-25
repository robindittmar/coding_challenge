cmake_minimum_required(VERSION 3.31)
project(greenbone_coding_challenge C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(LIBGVM_BASE REQUIRED libgvm_base>=22.17)
pkg_check_modules(LIBGVM_UTIL REQUIRED libgvm_util>=22.17)

add_executable(greenbone_coding_challenge src/main.c)

target_include_directories(greenbone_coding_challenge PRIVATE
    ${GLIB_INCLUDE_DIRS}
    ${GVM_BASE_INCLUDE_DIRS}
    ${GVM_UTIL_INCLUDE_DIRS})
target_link_libraries(greenbone_coding_challenge PkgConfig::deps gvm_base gvm_util)

