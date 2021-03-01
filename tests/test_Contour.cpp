//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-23.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Contour.hpp>
#include <catch2/catch.hpp>
#include <Chorasmia/Array2D.hpp>

TEST_CASE("Test getValue")
{
    Chorasmia::Array2D<double> grid({1, 2, 3,
                                     4, 5, 6,
                                     7, 8, 9},
                                    3, 3);
    auto value = GridLib::getValue(grid, 0.5, 0.5);
    REQUIRE(value == Approx(3));
}

TEST_CASE("Something")
{
    Chorasmia::Array2D<double> grid({1, 2, 3,
                                     4, 5, 6,
                                     7, 8, 9},
                                    3, 3);
    auto contour = GridLib::getContour(grid, 0.5, 0.5, 1.0, 1.0, 2);
    REQUIRE(contour.size() == 2);
    REQUIRE(contour[0] == Approx(3));
    REQUIRE(contour[1] == Approx(7));
}
