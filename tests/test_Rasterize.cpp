//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-07-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Grid.hpp>
#include <GridLib/Rasterize.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test get_index_mode_for_top_left_origin")
{
    using GridLib::get_index_mode_for_top_left_origin;
    using Chorasmia::Index2DMode;

    auto grid = [](const Xyz::Vector2D& r, const Xyz::Vector2D& c)
    {
        GridLib::Grid grid(1, 1);
        auto& model = grid.spatial_info();
        model.set_row_axis(Xyz::make_vector3(r, 0.0));
        model.set_column_axis(Xyz::make_vector3(c, 0.0));
        model.horizontal_unit = GridLib::Unit::METER;
        model.vertical_unit = GridLib::Unit::METER;
        return grid;
    };

    REQUIRE(get_index_mode_for_top_left_origin(grid({1, 0}, {0, -1}))
        == Chorasmia::Index2DMode::ROWS);
    REQUIRE(get_index_mode_for_top_left_origin(grid({1, 0}, {0, 1}))
        == Chorasmia::Index2DMode::ROWS_REVERSED_ORDER);
    REQUIRE(get_index_mode_for_top_left_origin(grid({-1, 0}, {0, -1}))
        == Chorasmia::Index2DMode::REVERSED_ROWS);
    REQUIRE(get_index_mode_for_top_left_origin(grid({-1, 0}, {0, 1}))
        == Chorasmia::Index2DMode::REVERSED_ROWS_REVERSED_ORDER);
    REQUIRE(get_index_mode_for_top_left_origin(grid({0, -1}, {1, 0}))
        == Chorasmia::Index2DMode::COLUMNS);
    REQUIRE(get_index_mode_for_top_left_origin(grid({0, 1}, {1, 0}))
        == Chorasmia::Index2DMode::COLUMNS_REVERSED_ORDER);
    REQUIRE(get_index_mode_for_top_left_origin(grid({0, -1}, {-1, 0}))
        == Chorasmia::Index2DMode::REVERSED_COLUMNS);
    REQUIRE(get_index_mode_for_top_left_origin(grid({0, 1}, {-1, 0}))
        == Chorasmia::Index2DMode::REVERSED_COLUMNS_REVERSED_ORDER);

    REQUIRE(get_index_mode_for_top_left_origin(grid({1, 0.99}, {0.99, -1}))
        == Chorasmia::Index2DMode::ROWS);
    REQUIRE(get_index_mode_for_top_left_origin(grid({1, 1.01}, {1.01, -1}))
        == Chorasmia::Index2DMode::COLUMNS_REVERSED_ORDER);
}
