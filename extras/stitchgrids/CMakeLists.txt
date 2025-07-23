# ===========================================================================
# Copyright © 2025 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2025-07-05.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.19)

include(FetchContent)
FetchContent_Declare(argos
    GIT_REPOSITORY "https://github.com/jebreimo/Argos.git"
    GIT_TAG v1.7.2)
FetchContent_Declare(fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG 7.1.3)
FetchContent_Declare(yglob
    GIT_REPOSITORY "https://github.com/jebreimo/Yglob.git"
    GIT_TAG "v0.1.0" ## Replace with most recent tag.
)
FetchContent_MakeAvailable(argos fmt yglob)

add_executable(stitchgrids
    stitchgrids.cpp)

target_link_libraries(stitchgrids
    Argos::Argos
    fmt::fmt
    GridLib::GridLib
    Xyz::Xyz
    Yglob::Yglob
)
