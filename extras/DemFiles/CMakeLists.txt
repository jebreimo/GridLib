# ===========================================================================
# Copyright © 2021 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2021-03-28.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)

add_library(GridLib_ReadDem OBJECT
    include/GridLib/ReadDem.hpp
    src/GridLib/DemReader.cpp
    src/GridLib/DemReader.hpp
    src/GridLib/FortranReader.cpp
    src/GridLib/FortranReader.hpp
    src/GridLib/ParseNumber.cpp
    src/GridLib/ParseNumber.hpp
    src/GridLib/PrintMacros.hpp
    src/GridLib/ReadDem.cpp
    src/GridLib/RecordA.cpp
    src/GridLib/RecordA.hpp
    src/GridLib/RecordB.cpp
    src/GridLib/RecordB.hpp
    src/GridLib/RecordC.cpp
    src/GridLib/RecordC.hpp
    )

target_include_directories(GridLib_ReadDem
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${GridLib_SOURCE_DIR}/include
    )

target_link_libraries(GridLib_ReadDem
    Chorasmia::Chorasmia
    Xyz::Xyz
    )

if (GridLib_INSTALL)
    install(TARGETS GridLib_ReadDem
        EXPORT GridLibConfig
        )

    install(
        DIRECTORY
            include/GridLib
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif ()
