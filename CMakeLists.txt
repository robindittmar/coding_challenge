cmake_minimum_required(VERSION 3.31)
project(greenbone_coding_challenge C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(CJSON REQUIRED libcjson>=1.7.14)
pkg_check_modules(GVM_BASE REQUIRED libgvm_base>=22.17)
pkg_check_modules(GVM_UTIL REQUIRED libgvm_util>=22.17)

add_executable(greenbone_coding_challenge src/main.c
        src/message.h
        src/message.c
        src/parse.h
        src/parse.c
        src/decode.h
        src/decode.c
        src/util.h
        src/util.c)

target_include_directories(greenbone_coding_challenge PRIVATE
    ${GLIB_INCLUDE_DIRS}
    ${GVM_BASE_INCLUDE_DIRS}
    ${GVM_UTIL_INCLUDE_DIRS})
target_link_libraries(greenbone_coding_challenge
        ${GLIB_LIBRARIES}
        ${CJSON_LIBRARIES}
        ${GVM_BASE_LIBRARIES}
        ${GVM_UTIL_LIBRARIES})

