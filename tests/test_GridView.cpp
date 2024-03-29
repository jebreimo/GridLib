//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-05-19.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/Grid.hpp>
#include <catch2/catch_test_macros.hpp>


TEST_CASE("test get_min_max_elevation")
{
    Chorasmia::Array2D<double> values({-999, -1, 4, 3, 1, -4}, 2, 3);
    GridLib::Grid grid(std::move(values));
    grid.set_unknown_elevation(-999);
    auto [min, max] = GridLib::get_min_max_elevation(grid.view());
    REQUIRE(min == -4);
    REQUIRE(max == 4);
}
