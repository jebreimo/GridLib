//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/ReadGrid.hpp>
#include <GridLib/WriteGrid.hpp>
#include <sstream>
#include <numeric>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Test write_json_grid and read_grid")
{
    GridLib::Grid grid;
    grid.resize(6, 8);
    auto array = grid.elevations().array();
    std::iota(array.begin(), array.end(), 0.0);
    grid.set_reference_system(GridLib::ReferenceSystem{3000, 1000});
    grid.set_planar_coords(GridLib::PlanarCoords{5000, 7000, 30});
    grid.set_row_axis(GridLib::Axis{{0, -10, 0}, GridLib::Unit::METERS});
    grid.set_column_axis(GridLib::Axis{{10, 0, 0}, GridLib::Unit::METERS});
    grid.set_vertical_axis(GridLib::Axis{{0, 0, 1}, GridLib::Unit::METERS});
    std::stringstream ss;
    GridLib::write_json_grid(ss, grid);
    auto inGrid = GridLib::read_json_grid(ss);
    REQUIRE(grid == inGrid);
}
