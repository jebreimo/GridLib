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
    grid.set_unknown_elevation(-999);
    auto [min, max] = get_min_max_elevation(grid.view());
    REQUIRE(min == -4);
    REQUIRE(max == 4);
}

TEST_CASE("test get_bounding_rect")
{
    Chorasmia::Array2D<float> values({-999, -1, 4, 3, 1, -4}, 2, 3);
    GridLib::Grid grid(std::move(values));
    grid.set_planar_coords(GridLib::PlanarCoords{500000, 6000000});
    grid.set_row_axis({{10, 0, 0}, GridLib::Unit::METER});
    grid.set_column_axis({{0, -10, 0}, GridLib::Unit::METER});
    grid.set_unknown_elevation(-999);
    auto rect = get_bounding_rect(grid.view());
    REQUIRE(rect.origin == Xyz::Vector2D(500000, 6000000));
    REQUIRE(rect.size == Xyz::Vector2D(30, -20));
}

TEST_CASE("test get_elevation")
{
    using Catch::Matchers::WithinAbs;
    Chorasmia::Array2D<float> values({-1, 4, 3, -999, 1, -4}, 2, 3);
    GridLib::Grid grid(std::move(values));
    grid.set_unknown_elevation(-999);
    auto view = grid.view();

    REQUIRE(get_elevation(view, {0, 0}) == -1);
    REQUIRE_THAT(get_elevation(view, {0, 0.5}), WithinAbs(1.5, 1.0e-5));
    REQUIRE(get_elevation(view, {0, 1}) == 4);
    REQUIRE_THAT(get_elevation(view, {0, 1.5}), WithinAbs(3.5, 1.0e-5));
    REQUIRE(get_elevation(view, {0, 2}) == 3);

    REQUIRE(get_elevation(view, {0.5, 0}) == -999);
    REQUIRE(get_elevation(view, {0.5, 0.5}) == -999);
    REQUIRE_THAT(get_elevation(view, {0.5, 1}), WithinAbs(2.5, 1.0e-5));
    REQUIRE_THAT(get_elevation(view, {0.5, 1.5}), WithinAbs(1, 1.0e-5));
    REQUIRE_THAT(get_elevation(view, {0.5, 2}), WithinAbs(-0.5, 1.0e-5));

    REQUIRE(get_elevation(view, {1, 0}) == -999);
    REQUIRE(get_elevation(view, {1, 0.5}) == -999);
    REQUIRE(get_elevation(view, {1, 1}) == 1);
    REQUIRE_THAT(get_elevation(view, {1, 1.5}), WithinAbs(-1.5, 1.0e-5));
    REQUIRE_THAT(get_elevation(view, {1, 2}), WithinAbs(-4, 1.0e-5));

    REQUIRE_THROWS(get_elevation(view, {-1, 0}));
    REQUIRE_THROWS(get_elevation(view, {0, -1}));
    REQUIRE_THROWS(get_elevation(view, {0, 3}));
    REQUIRE_THROWS(get_elevation(view, {2, 0}));
}

TEST_CASE("test get_elevation with only left and top edges")
{
    using Catch::Matchers::WithinAbs;
    Chorasmia::Array2D<float> values({-1, 4, 3, -999}, 2, 2);
    GridLib::Grid grid(std::move(values));
    grid.set_unknown_elevation(-999);
    auto view = grid.view();

    REQUIRE(get_elevation(view, {0, 0}) == -1);
    REQUIRE_THAT(get_elevation(view, {0, 0.5}), WithinAbs(1.5, 1.0e-5));
    REQUIRE(get_elevation(view, {0, 1}) == 4);

    REQUIRE_THAT(get_elevation(view, {0.5, 0}), WithinAbs(1, 1.0e-5));
    REQUIRE(get_elevation(view, {0.5, 1}) == -999);

    REQUIRE(get_elevation(view, {1, 0}) == 3);
    REQUIRE(get_elevation(view, {1, 0.5}) == -999);
    REQUIRE(get_elevation(view, {1, 1}) == -999);
}

TEST_CASE("test model_pos_to_grid_pos")
{
    GridLib::Grid grid(10, 10);
    grid.set_planar_coords(GridLib::PlanarCoords{1000, 10000, 0});
    grid.set_row_axis({{10, 0, 0}, GridLib::Unit::METER});
    grid.set_column_axis({{0, -10, 0}, GridLib::Unit::METER});
    auto view = grid.view();

    using V = Xyz::Vector2F;
    REQUIRE(are_equal(model_pos_to_grid_pos(view, {1000, 10000, 0}), V(0, 0)));
    REQUIRE(are_equal(model_pos_to_grid_pos(view, {1000, 9990, 0}),  V(1, 0)));
    REQUIRE(are_equal(model_pos_to_grid_pos(view, {1037, 9942, 0}),  V(5.8, 3.7)));
}
