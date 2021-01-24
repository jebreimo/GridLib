# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-08-28.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)
project(GridLib VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(chorasmia
    GIT_REPOSITORY "https://github.com/jebreimo/Chorasmia.git"
    GIT_TAG v0.1.2)
FetchContent_Declare(yson
    GIT_REPOSITORY "https://github.com/jebreimo/Yson.git"
    GIT_TAG v1.0.7)
FetchContent_MakeAvailable(yson chorasmia)

function(GridLib_enable_all_warnings target)
    target_compile_options(${target}
        PRIVATE
            $<$<CXX_COMPILER_ID:Clang>:-Wall>
            $<$<CXX_COMPILER_ID:GNU>:-Wall>
            $<$<CXX_COMPILER_ID:AppleClang>:-Wall>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>)
endfunction()

add_library(GridLib
    include/GridLib/Grid.hpp
    include/GridLib/GridLibException.hpp
    include/GridLib/GridMemberTypes.hpp
    include/GridLib/GridView.hpp
    include/GridLib/ReadGrid.hpp
    include/GridLib/WriteGrid.hpp
    src/GridLib/Grid.cpp
    src/GridLib/GridMemberTypes.cpp
    src/GridLib/GridView.cpp
    src/GridLib/ReadGrid.cpp
    src/GridLib/WriteGrid.cpp
    )

target_include_directories(GridLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(GridLib
    PUBLIC
        Chorasmia::Chorasmia
    PRIVATE
        Yson::Yson
    )

GridLib_enable_all_warnings(GridLib)

add_library(GridLib::GridLib ALIAS GridLib)

enable_testing()

add_subdirectory(tests)

export(TARGETS GridLib
    NAMESPACE GridLib::
    FILE GridLibConfig.cmake)
