//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-11-26.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <GridLib/MultiGridReader.hpp>
#include "TestData.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers.hpp>

using Catch::Matchers::WithinAbs;

TEST_CASE("Test MultiGridReader")
{
    using namespace GridLib;

    MultiGridReader reader;
    reader.read_grid(GEOTIFF_FILE_1.data(), GEOTIFF_FILE_1.size(), GridFileType::GEOTIFF);
    reader.read_grid(GEOTIFF_FILE_2.data(), GEOTIFF_FILE_2.size(), GridFileType::GEOTIFF);

    REQUIRE(reader.size().rows == 300);
    REQUIRE(reader.size().columns == 560);

    REQUIRE(reader.has_data({{0, 0}, {SIZE_MAX, SIZE_MAX}}));
    REQUIRE(!reader.has_data({{0, 0}, {60, 80}}));
    REQUIRE(reader.has_data({{240, 0}, {60, 80}}));
    REQUIRE(!reader.has_data({{240, 480}, {60, 80}}));
    REQUIRE(reader.has_data({{0, 480}, {60, 80}}));

    auto grid = reader.get_grid({{220, 10}, {40, 40}});
    REQUIRE(grid.size().rows == 40);
    REQUIRE(grid.size().columns == 40);
    REQUIRE(grid[{0, 0}] == GridLib::UNKNOWN_ELEVATION);
    auto h = grid[{20, 20}];
    REQUIRE_THAT(h, WithinAbs(42.50124f, 0.001f));
}
