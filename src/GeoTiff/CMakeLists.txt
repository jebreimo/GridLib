# ===========================================================================
# Copyright © 2025 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2025-01-05.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.30)

include(FetchContent)

add_library(GridLib_GeoTiff OBJECT
    include/GridLib/ReadGeoTiff.hpp
    src/GridLib/ReadGeoTiff.cpp
)

target_include_directories(GridLib_GeoTiff
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/GeoTiff/include>
        $<BUILD_INTERFACE:${GridLib_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${GridLib_SOURCE_DIR}/src>
)

target_link_libraries(GridLib_GeoTiff
    PRIVATE
        Chorasmia::Chorasmia
        Xyz::Xyz
        Yimage::Yimage
)

if (GridLib_INSTALL)
    install(TARGETS GridLib_GeoTiff
        EXPORT GridLibConfig
    )

    install(
        DIRECTORY
            include/GridLib
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif ()
