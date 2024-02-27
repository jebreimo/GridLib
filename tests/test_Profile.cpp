//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Profile.hpp>
#include <Chorasmia/Array2D.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinAbs;

TEST_CASE("Test getValue")
{
    Chorasmia::Array2D<double> grid({1, 2, 3,
                                     4, 5, 6,
                                     7, 8, 9},
                                     3, 3);
    auto value = GridLib::getValue(grid.view(), 0.5, 0.5);
    REQUIRE_THAT(value, WithinAbs(3, 1e-12));
}

TEST_CASE("Something")
{
    Chorasmia::Array2D<double> grid({1, 2, 3,
                                     4, 5, 6,
                                     7, 8, 9},
                                    3, 3);
    auto contour = GridLib::makeProfile(grid.view(), {0.5, 0.5}, {1.5, 1.5}, 2);
    REQUIRE(contour.size() == 3);
    REQUIRE_THAT(contour[0][2], WithinAbs(3, 1e-12));
    REQUIRE_THAT(contour[1][2], WithinAbs(5, 1e-12));
    REQUIRE_THAT(contour[2][2], WithinAbs(7, 1e-12));
}
