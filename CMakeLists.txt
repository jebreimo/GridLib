# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-08-28.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)
project(GridLib VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 20)

# Set GridLib_MASTER_PROJECT to ON if Argos is the root level project.
if (NOT DEFINED GridLib_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(GridLib_MASTER_PROJECT ON)
    else ()
        set(GridLib_MASTER_PROJECT OFF)
    endif ()
endif ()

# Test option
option(GridLib_BUILD_TEST "Build tests" ${GridLib_MASTER_PROJECT})

# Install option
option(GridLib_INSTALL "Generate the install target" ${GridLib_MASTER_PROJECT})

# DEM file option
option(GridLib_DEM_SUPPORT "Enable support for DEM files" ON)

if (GridLib_INSTALL)
    set(CHORASMIA_INSTALL ON)
    set(YSON_INSTALL ON)
    set(XYZ_INSTALL ON)
endif ()

include(FetchContent)
FetchContent_Declare(chorasmia
    GIT_REPOSITORY "https://github.com/jebreimo/Chorasmia.git"
    GIT_TAG master)
FetchContent_Declare(xyz
    GIT_REPOSITORY "https://github.com/jebreimo/Xyz.git"
    GIT_TAG v0.3.2)
FetchContent_Declare(yson
    GIT_REPOSITORY "https://github.com/jebreimo/Yson.git"
    GIT_TAG v1.4.16)
FetchContent_MakeAvailable(chorasmia xyz yson)

function(GridLib_enable_all_warnings target)
    target_compile_options(${target}
        PRIVATE
            $<$<CXX_COMPILER_ID:Clang>:-Wall>
            $<$<CXX_COMPILER_ID:GNU>:-Wall>
            $<$<CXX_COMPILER_ID:AppleClang>:-Wall>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>)
endfunction()

set(GridLib_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (GridLib_DEM_SUPPORT)
    add_subdirectory(extras/DemFiles)
endif ()

include(GNUInstallDirs)

add_library(GridLib
    include/GridLib/Grid.hpp
    include/GridLib/GridLibException.hpp
    include/GridLib/GridMemberTypes.hpp
    include/GridLib/GridView.hpp
    include/GridLib/Profile.hpp
    include/GridLib/ReadGrid.hpp
    include/GridLib/WriteGrid.hpp
    src/GridLib/Grid.cpp
    src/GridLib/GridMemberTypes.cpp
    src/GridLib/GridView.cpp
    src/GridLib/Profile.cpp
    src/GridLib/ReadGrid.cpp
    src/GridLib/WriteGrid.cpp
    src/GridLib/Rasterize.cpp
    include/GridLib/Rasterize.hpp
    )

target_include_directories(GridLib
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

target_compile_definitions(GridLib
    PUBLIC
        $<$<BOOL:GridLib_DEM_SUPPORT>:GRIDLIB_DEM_SUPPORT>
    )

target_link_libraries(GridLib
    PUBLIC
        Chorasmia::Chorasmia
        Xyz::Xyz
        $<$<BOOL:GridLib_DEM_SUPPORT>:GridLib_ReadDem>
    PRIVATE
        Yson::Yson
    )

GridLib_enable_all_warnings(GridLib)

add_library(GridLib::GridLib ALIAS GridLib)

if (GridLib_BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif ()

export(TARGETS GridLib
    NAMESPACE GridLib::
    FILE GridLibConfig.cmake)

if (GridLib_INSTALL)
    install(TARGETS GridLib
        EXPORT GridLibConfig
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

    install(EXPORT GridLibConfig
        FILE
            GridLibConfig.cmake
        NAMESPACE
            GridLib::
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/GridLib
        )

    install(
        DIRECTORY
        include/GridLib
        DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    )

    #    install(
    #        FILES
    #            ${CMAKE_CURRENT_BINARY_DIR}/GridLibVersion.hpp
    #        DESTINATION
    #            ${CMAKE_INSTALL_INCLUDEDIR}/GridLib
    #    )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(GridLibConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
        )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GridLibConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/GridLib
        )
endif ()
