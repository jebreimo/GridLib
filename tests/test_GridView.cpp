//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-19.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Grid.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("test get_min_max_elevation")
{
    Chorasmia::Array2D<float> values({-999, -1, 4, 3, 1, -4}, 2, 3);
    GridLib::Grid grid(std::move(values));
    grid.spatial_info().unknown_elevation = -999.f;
    auto [min, max] = get_min_max_elevation(grid);
    REQUIRE(min == -4);
    REQUIRE(max == 4);
}

TEST_CASE("test get_bounding_rect")
{
    Chorasmia::Array2D<float> values({-999, -1, 4, 3, 1, -4}, 2, 3);
    GridLib::Grid grid(std::move(values));
    auto& model = grid.spatial_info();
    model.set_location({500000, 6000000, 0});
    model.horizontal_unit = GridLib::Unit::METER;
    model.vertical_unit = GridLib::Unit::METER;
    model.set_row_axis({10, 0, 0});
    model.set_column_axis({0, -10, 0});
    model. unknown_elevation = -999.f;
    auto rect = get_bounds(grid);
    using V3 = Xyz::Vector3D;
    REQUIRE(rect.origin == V3(500000, 6000000, 0));
    REQUIRE(get_length(rect.edge0) == 10);
    REQUIRE(get_length(rect.edge1) == 20);
}
