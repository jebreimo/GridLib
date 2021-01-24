//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/ReadGrid.hpp>
#include <GridLib/WriteGrid.hpp>
#include <catch2/catch.hpp>

#include <sstream>

TEST_CASE("Test writeJsonGrid and readGrid")
{
    GridLib::Grid grid;
    grid.resize(6, 8);
    auto array = grid.elevations().array();
    std::iota(array.begin(), array.end(), 0.0);
    grid.setReferenceSystem(GridLib::ReferenceSystem{3000, 1000});
    grid.setPlanarCoords(GridLib::PlanarCoords{5000, 7000, 30});
    grid.setRowAxis(GridLib::Axis{10, GridLib::Unit::METERS});
    grid.setColumnAxis(GridLib::Axis{10, GridLib::Unit::METERS});
    grid.setVerticalAxis(GridLib::Axis{1, GridLib::Unit::METERS});
    std::stringstream ss;
    GridLib::writeJsonGrid(ss, grid);
    auto inGrid = GridLib::readJsonGrid(ss);
    REQUIRE(grid == inGrid);
}
