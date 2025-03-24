//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Grid.hpp>
#include <GridLib/Profile.hpp>
#include <Chorasmia/Array2D.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

TEST_CASE("Test get_value")
{
    GridLib::Grid grid({{1, 2, 3,
                         4, 5, 6,
                         7, 8, 9},
                        3, 3});
    auto value = GridLib::get_value(grid.view(), {0.5, 0.5});
    REQUIRE_THAT(value, WithinAbs(3, 1e-12));
}

TEST_CASE("Profile without clipping")
{
    Chorasmia::Array2D<double> grid({1, 2, 3,
                                     4, 5, 6,
                                     7, 8, 9},
                                    3, 3);
    auto contour = GridLib::make_profile(grid.view(), {0.5, 0.5}, {1.5, 1.5}, 2);
    REQUIRE(contour.size() == 3);
    REQUIRE_THAT(contour[0][2], WithinAbs(3, 1e-12));
    REQUIRE_THAT(contour[1][2], WithinAbs(5, 1e-12));
    REQUIRE_THAT(contour[2][2], WithinAbs(7, 1e-12));
}

TEST_CASE("Profile with clipping")
{
    Chorasmia::Array2D<double> grid({1, 2, 3,
                                     4, 5, 6,
                                     7, 8, 9},
                                    3, 3);
    auto contour = GridLib::make_profile(grid.view(), {4.5, 3}, {-2.5, -0.5}, 7);
    REQUIRE(contour.size() == 4);
    REQUIRE_THAT(contour[0][2], WithinAbs(8.75, 1e-12));
    REQUIRE_THAT(contour[1][2], WithinAbs(7, 1e-12));
    REQUIRE_THAT(contour[2][2], WithinAbs(3.5, 1e-12));
    REQUIRE_THAT(contour[3][2], WithinAbs(1.75, 1e-12));
}

TEST_CASE("Profile with transformation")
{
    Chorasmia::Array2D<double> array({1, 2, 3,
                                      4, 5, 6,
                                      7, 8, 9},
                                     3, 3);
    GridLib::Grid grid(array);
    grid.set_planar_coords(GridLib::PlanarCoords{500.0, 1000.0});
    grid.set_row_axis({{10, 0, 0}, GridLib::Unit::METERS});
    grid.set_column_axis({{0, -10, 0}, GridLib::Unit::METERS});
    grid.set_vertical_axis({{0, 0, 10}, GridLib::Unit::METERS});

    GridLib::ProfileMaker maker(grid.view());

    auto contour = maker.make_profile({530, 955}, {495, 1025}, 7);
    REQUIRE(contour.size() == 4);
    REQUIRE(Xyz::are_equal(contour[0], {517.5, 980, 87.5}));
    REQUIRE(Xyz::are_equal(contour[1], {515, 985, 70}));
    REQUIRE(Xyz::are_equal(contour[2], {510, 995, 35}));
    REQUIRE(Xyz::are_equal(contour[3], {507.5, 1000, 17.5}));
}
