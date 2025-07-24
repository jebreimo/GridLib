# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-08-28.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.13)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v3.5.2"
)
FetchContent_Declare(cppembed
    GIT_REPOSITORY "https://github.com/jebreimo/cppembed.git"
    GIT_TAG "v0.1.3"
)
FetchContent_MakeAvailable(catch cppembed)

list(APPEND CMAKE_MODULE_PATH ${cppembed_SOURCE_DIR}/cmake)

include(TargetEmbedCppData)

add_executable(GridLibTest
    test_GridView.cpp
    test_Profile.cpp
    test_Rasterize.cpp
    test_ReadAndWriteGrid.cpp
    test_ReadDem.cpp
    test_ReadGeoTiff.cpp
    test_PositionTransformer.cpp
    test_GridInterpolator.cpp
)

target_link_libraries(GridLibTest
    PRIVATE
        GridLib::GridLib
        Catch2::Catch2WithMain
)

target_embed_cpp_data(GridLibTest
    FILES
        dem_file.hpp.in
        geotiff_file.hpp.in
)

target_include_directories(GridLibTest
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

GridLib_enable_all_warnings(GridLibTest)

add_test(NAME GridLibTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/GridLibTest)
