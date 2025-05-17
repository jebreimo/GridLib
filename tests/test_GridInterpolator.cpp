//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-17.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/GridInterpolator.hpp"

#include "GridLib/Grid.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

namespace
{
    void check_grid_pos(const GridLib::GridInterpolator& interpolator,
                        const Xyz::Vector2D& pos,
                        const Xyz::Vector3D& expected)
    {
        CAPTURE(pos);
        const auto result = interpolator.at_grid_pos(pos);
        REQUIRE(result.has_value());
        CHECK(Xyz::are_equal(result.value(), expected));
    }

    void check_model_pos(const GridLib::GridInterpolator& interpolator,
                        const Xyz::Vector3D& pos,
                        const Xyz::Vector3D& expected)
    {
        CAPTURE(pos);
        const auto result = interpolator.at_model_pos(pos);
        REQUIRE(result.has_value());
        CHECK(Xyz::are_equal(result.value(), expected));
    }
}

TEST_CASE("GridInterpolator::at_grid_pos with missing elevation")
{
    Chorasmia::Array2D<float> values({-1, 4, 3, -999, 1, -4}, 2, 3);
    GridLib::Grid grid(std::move(values));
    grid.model().unknown_elevation = -999;
    GridLib::GridInterpolator interpolator(grid);

    check_grid_pos(interpolator, {0, 0}, {0, 0, -1});
    check_grid_pos(interpolator, {0, 0.5}, {0, 0.5, 1.5});
    check_grid_pos(interpolator, {0, 1}, {0, 1, 4});
    check_grid_pos(interpolator, {0, 1.5}, {0, 1.5, 3.5});
    check_grid_pos(interpolator, {0, 2}, {0, 2, 3});

    CHECK(!interpolator.at_grid_pos({0.5, 0}));
    CHECK(!interpolator.at_grid_pos({0.5, 0.5}));
    check_grid_pos(interpolator, {0.5, 1}, {0.5, 1, 2.5});
    check_grid_pos(interpolator, {0.5, 1.5}, {0.5, 1.5, 1});
    check_grid_pos(interpolator, {0.5, 2}, {0.5, 2, -0.5});

    CHECK(!interpolator.at_grid_pos({1, 0}));
    CHECK(!interpolator.at_grid_pos({1, 0.5}));
    check_grid_pos(interpolator, {1, 1}, {1, 1, 1});
    check_grid_pos(interpolator, {1, 1.5}, {1, 1.5, -1.5});
    check_grid_pos(interpolator, {1, 2}, {1, 2, -4});

    CHECK(!interpolator.at_grid_pos({-1, 0}));
    CHECK(!interpolator.at_grid_pos({0, -1}));
    CHECK(!interpolator.at_grid_pos({0, 3}));
    CHECK(!interpolator.at_grid_pos({2, 0}));
}

TEST_CASE("GridInterpolator::at_grid_pos with only left and top edges")
{
    Chorasmia::Array2D<float> values({-1, 4, 3, -999}, 2, 2);
    GridLib::Grid grid(std::move(values));
    grid.model().unknown_elevation = -999;
    GridLib::GridInterpolator interpolator(grid);

    check_grid_pos(interpolator, {0, 0}, {0, 0, -1});
    check_grid_pos(interpolator, {0, 0.5}, {0, 0.5, 1.5});
    check_grid_pos(interpolator, {0, 1}, {0, 1, 4});

    check_grid_pos(interpolator, {0.5, 0}, {0.5, 0, 1});
    CHECK(!interpolator.at_grid_pos({0.5, 1}));

    check_grid_pos(interpolator, {1, 0}, {1, 0, 3});
    CHECK(!interpolator.at_grid_pos({1, 0.5}));
    CHECK(!interpolator.at_grid_pos({1, 1}));
}

TEST_CASE("GridInterpolator::at_model_pos")
{
    Chorasmia::Array2D<float> values({0, 10, 30, 40}, 2, 2);
    GridLib::Grid grid(std::move(values));
    auto& model = grid.model();
    model.set_column_axis({0, -10, 0});
    model.set_row_axis({10, 0, 0});
    model.set_vertical_axis({0, 0, 0.25});
    model.set_location({10'000, 100'000, 150});
    grid.set_model_tie_point({1, 1});
    GridLib::GridInterpolator interpolator(grid);

    check_model_pos(interpolator, {9'995, 100'005, 0}, {9'995, 100'005, 155});
}
