//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/PositionTransformer.hpp"

#include "GridLib/Grid.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PositionTransformer: Origin in top left corner")
{
    GridLib::Grid grid(10, 10);
    auto& model = grid.spatial_info();
    model.set_column_axis({0, -10, 0});
    model.set_row_axis({10, 0, 0});
    model.set_vertical_axis({0, 0, 1});
    model.set_location({10'000, 100'000, 150});

    const GridLib::PositionTransformer transformer(grid);

    auto p3 = transformer.grid_to_model({1, 2});
    REQUIRE(p3 == Xyz::Vector3D(10'020, 99'990, 150));

    p3 = transformer.grid_to_model({1, 2, 200});
    REQUIRE(p3 == Xyz::Vector3D(10'020, 99'990, 350));

    auto p2 = transformer.model_to_grid({10'035, 99'935, 200});
    REQUIRE(p2 == Xyz::Vector2D(6.5, 3.5));
}

TEST_CASE("PositionTransformer: Offset tie-point")
{
    GridLib::Grid grid(10, 10);
    grid.set_tie_point({4.5, 3});

    const GridLib::PositionTransformer transformer(grid);

    auto p3 = transformer.grid_to_model({1, 2});
    REQUIRE(p3 == Xyz::Vector3D(-3.5, -1, 0));

    p3 = transformer.grid_to_model({1, 2, 200});
    REQUIRE(p3 == Xyz::Vector3D(-3.5, -1, 200));

    auto p2 = transformer.model_to_grid({6, 6, 100});
    REQUIRE(p2 == Xyz::Vector2D(10.5, 9));
}
