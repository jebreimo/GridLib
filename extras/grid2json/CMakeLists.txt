# ===========================================================================
# Copyright © 2025 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2025-02-11.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)
project(grid2json VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 20)

add_executable(grid2json
    grid2json.cpp
)

target_link_libraries(grid2json
    GridLib::GridLib
)
