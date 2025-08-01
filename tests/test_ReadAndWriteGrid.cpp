//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/ReadJsonGrid.hpp>
#include <GridLib/WriteJsonGrid.hpp>

#include <numeric>
#include <sstream>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test write_json_grid and read_grid")
{
    GridLib::Grid grid;
    grid.resize(6, 8);
    auto array = grid.values().array();
    std::iota(array.begin(), array.end(), 0.f);
    grid.set_tie_point({2, 3});
    auto& model = grid.spatial_info();
    model.crs = {
        3000, 1000,
        GridLib::CrsType::PROJECTED, GridLib::CrsLibrary::EPSG,
        "EPSG:3000"
    };
    model.set_location({5000, 7000, 0});
    model.horizontal_unit = GridLib::Unit::METER;
    model.vertical_unit = GridLib::Unit::METER;
    model.set_row_axis({0, -10, 0});
    model.set_column_axis({10, 0, 0});
    model.set_vertical_axis({0, 0, 1});
    std::vector<GridLib::SpatialTiePoint> tie_points;
    tie_points.push_back({
        grid.tie_point(),
        model.location(),
        model.crs
    });
    grid.set_spatial_tie_points(std::move(tie_points));
    std::stringstream ss;
    GridLib::write_json(ss, grid);
    ss.seekg(0);
    auto in_grid = GridLib::read_json_grid(ss, true);
    REQUIRE(grid == in_grid);
}
