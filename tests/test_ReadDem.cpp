//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "dem_file.hpp"
#include "GridLib/ReadGrid.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Read DEM file")
{
    auto grid = GridLib::read_grid(DEM_FILE.data(), DEM_FILE.size(),
                                   GridLib::GridFileType::DEM);
    REQUIRE(grid.row_count() == 370);
    REQUIRE(grid.col_count() == 313);
    REQUIRE(grid.model().horizontal_unit == GridLib::Unit::METER);
    REQUIRE(grid.model().vertical_unit == GridLib::Unit::METER);
    REQUIRE(grid.spatial_tie_points().size() == 4);
    REQUIRE(Xyz::are_equal(grid.model().column_axis(), {1, 0, 0}));
    REQUIRE(Xyz::are_equal(grid.model().row_axis(), {0, 1, 0}));
    REQUIRE(Xyz::are_equal(grid.model().vertical_axis(), {0, 0, 0.1}));
}
