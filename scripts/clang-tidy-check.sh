#!/bin/bash
BUILD_DIR=${1:-build}
shift 1
CHECK_DIR_LIST=$@
CHECK_LIST=( \
       'clang-analyzer-*' \
       'clang-analyzer-cplusplus*' \
       'concurrency-*' \
       'cppcoreguidelines-*' \
       '-cppcoreguidelines-special-member-functions' \
       '-cppcoreguidelines-macro-usage' \
       '-cppcoreguidelines-pro-bounds-array-to-pointer-decay' \
       'modernize-*' \
       '-modernize-use-nodiscard' \
       '-modernize-use-trailing-return-type' \
       'performance-*' \
)
CHECK_LIST_JOINED=$(printf "%s," "${CHECK_LIST[@]}")
find ${CHECK_DIR_LIST} | \
    grep -E "\\.hpp|\\.cpp" | \
    grep -vi test | \
    xargs clang-tidy -p ${BUILD_DIR} --checks="-*,${CHECK_LIST_JOINED}"