# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-08-28.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.13)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v2.11.1"
    )
FetchContent_MakeAvailable(catch)

add_executable(GridLibTest
    GridLibTestMain.cpp
    test_ReadAndWriteGrid.cpp
    )

target_link_libraries(GridLibTest
    PRIVATE
        GridLib::GridLib
        Catch2::Catch2
    )

GridLib_enable_all_warnings(GridLibTest)

add_test(NAME GridLibTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/GridLibTest)
