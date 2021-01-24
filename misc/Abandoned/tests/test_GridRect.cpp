//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../include/GridLib/GridRect.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Test get_intersection")
{
    using namespace GridLib;
    REQUIRE(get_intersection({{5, 5}, {8, 8}}, {{3, 6}, {6, 10}})
            == GridRect{{5, 6}, {4, 7}});
}
