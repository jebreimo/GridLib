//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-23.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Grid.hpp>
#include <GridLib/Profile.hpp>
#include <Chorasmia/Array2D.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

TEST_CASE("Profile without clipping")
{
    Chorasmia::Array2D<float> values({
                                         1, 2, 3,
                                         4, 5, 6,
                                         7, 8, 9
                                     },
                                     3, 3);
    GridLib::Grid grid(std::move(values));
    auto contour = GridLib::make_profile(grid, {0, 2, 0}, {2, 0, 0}, 2);
    REQUIRE(contour.size() == 3);
    CHECK_THAT(contour[0][2], WithinAbs(3, 1e-12));
    CHECK_THAT(contour[1][2], WithinAbs(5, 1e-12));
    CHECK_THAT(contour[2][2], WithinAbs(7, 1e-12));
}

TEST_CASE("Profile with clipping")
{
    Chorasmia::Array2D<float> values({
                                         1, 2, 3,
                                         4, 5, 6,
                                         7, 8, 9
                                     },
                                     3, 3);
    GridLib::Grid grid(std::move(values));
    auto contour = GridLib::make_profile(grid, {4.5, 3, 0}, {-2.5, -0.5, 0}, 7);
    REQUIRE(contour.size() == 4);
    CHECK_THAT(contour[0][2], WithinAbs(8.75, 1e-12));
    CHECK_THAT(contour[1][2], WithinAbs(7, 1e-12));
    CHECK_THAT(contour[2][2], WithinAbs(3.5, 1e-12));
    CHECK_THAT(contour[3][2], WithinAbs(1.75, 1e-12));
}

TEST_CASE("Profile with clipping where clipping coincides with grid edge")
{
    Chorasmia::Array2D<float> values({
                                         1, 2, 3,
                                         4, 5, 6,
                                         7, 8, 9
                                     },
                                     3, 3);
    GridLib::Grid grid(std::move(values));
    auto contour = GridLib::make_profile(grid, {4.5, 3, 0}, {-2.5, -0.5, 0}, 7);
    REQUIRE(contour.size() == 4);
    CHECK_THAT(contour[0][2], WithinAbs(8.75, 1e-12));
    CHECK_THAT(contour[1][2], WithinAbs(7, 1e-12));
    CHECK_THAT(contour[2][2], WithinAbs(3.5, 1e-12));
    CHECK_THAT(contour[3][2], WithinAbs(1.75, 1e-12));
}

TEST_CASE("Profile with transformation")
{
    Chorasmia::Array2D<float> array({
                                        1, 2, 3,
                                        4, 5, 6,
                                        7, 8, 9
                                    },
                                    3, 3);
    GridLib::Grid grid(array);
    auto& model = grid.model();
    model.set_location({500.0, 1000.0, 0});
    model.horizontal_unit = GridLib::Unit::METER;
    model.vertical_unit = GridLib::Unit::METER;
    model.set_row_axis({10, 0, 0});
    model.set_column_axis({0, -10, 0});
    model.set_vertical_axis({0, 0, 10});

    GridLib::ProfileMaker maker(grid);

    auto contour = maker.make_profile({530, 955, 0}, {495, 1025, 0}, 7);
    REQUIRE(contour.size() == 4);
    CHECK(Xyz::are_equal(contour[0], {517.5, 980, 87.5}));
    CHECK(Xyz::are_equal(contour[1], {515, 985, 70}));
    CHECK(Xyz::are_equal(contour[2], {510, 995, 35}));
    CHECK(Xyz::are_equal(contour[3], {507.5, 1000, 17.5}));
}
