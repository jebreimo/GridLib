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
FetchContent_Declare(yglob
    GIT_REPOSITORY "https://github.com/jebreimo/Yglob.git"
    GIT_TAG "v0.1.1"
)
FetchContent_MakeAvailable(argos yglob)

add_executable(stitchgrids
    stitchgrids.cpp)

target_link_libraries(stitchgrids
    Argos::Argos
    GridLib::GridLib
    Xyz::Xyz
    Yglob::Yglob
)
