//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/ReadGrid.hpp>
#include <GridLib/WriteGrid.hpp>

#include <numeric>
#include <sstream>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Test write_json_grid and read_grid")
{
    GridLib::Grid grid;
    grid.resize(6, 8);
    auto array = grid.elevations().array();
    std::iota(array.begin(), array.end(), 0.0);
    grid.set_reference_system(GridLib::CoordinateReferenceSystem{3000, 1000});
    grid.set_coordinates({{5000, 7000, 0}});
    grid.set_horizontal_unit(GridLib::Unit::METER);
    grid.set_vertical_unit(GridLib::Unit::METER);
    grid.set_row_axis({0, -10, 0});
    grid.set_column_axis({10, 0, 0});
    grid.set_vertical_axis({0, 0, 1});
    std::stringstream ss;
    GridLib::write_json(ss, grid.view());
    ss.seekg(0);
    auto in_grid = GridLib::read_json_grid(ss, true);
    REQUIRE(grid == in_grid);
}
