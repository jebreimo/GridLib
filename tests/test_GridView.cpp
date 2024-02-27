//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Grid.hpp>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("test getMinMaxElevation")
{
    Chorasmia::Array2D<double> values({-999, -1, 4, 3, 1, -4}, 2, 3);
    GridLib::Grid grid(std::move(values));
    grid.setUnknownElevation(-999);
    auto [min, max] = GridLib::getMinMaxElevation(grid);
    REQUIRE(min == -4);
    REQUIRE(max == 4);
}
