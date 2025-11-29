//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TestData.hpp"
#include "GridLib/ReadGrid.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Read DEM file")
{
    auto grid = GridLib::read_grid(DEM_FILE.data(), DEM_FILE.size(),
                                   GridLib::GridFileType::DEM);
    REQUIRE(grid.size() == GridLib::Size(370, 313));
    REQUIRE(grid.spatial_info().horizontal_unit == GridLib::Unit::METER);
    REQUIRE(grid.spatial_info().vertical_unit == GridLib::Unit::METER);
    REQUIRE(grid.spatial_info().extra_tie_points.size() == 4);
    REQUIRE(Xyz::are_equal(grid.spatial_info().column_axis(), {1, 0, 0}));
    REQUIRE(Xyz::are_equal(grid.spatial_info().row_axis(), {0, 1, 0}));
    REQUIRE(Xyz::are_equal(grid.spatial_info().vertical_axis(), {0, 0, 0.1}));
    auto& crs = grid.spatial_info().crs;
    REQUIRE(crs.code == 32632);
    REQUIRE(crs.type == GridLib::CrsType::PROJECTED);
    REQUIRE(crs.library == GridLib::CrsLibrary::EPSG);
}
