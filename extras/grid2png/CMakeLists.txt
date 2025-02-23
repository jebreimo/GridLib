# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-07-20.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.19)

include(FetchContent)
FetchContent_Declare(fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG 7.1.3)
FetchContent_Declare(argos
    GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
    GIT_TAG v1.7.2)
FetchContent_MakeAvailable(argos fmt)

add_executable(grid2png
    grid2png.cpp)

target_link_libraries(grid2png
    Argos::Argos
    fmt::fmt
    GridLib::GridLib
    )
