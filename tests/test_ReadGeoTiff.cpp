//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "geotiff_file.hpp"
#include "GridLib/ReadGrid.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Read GeoTIFF file")
{
    auto grid = GridLib::read_grid(GEOTIFF_FILE.data(), GEOTIFF_FILE.size(),
                                   GridLib::GridFileType::GEOTIFF);
    REQUIRE(grid.row_count() == 60);
    REQUIRE(grid.col_count() == 80);
    REQUIRE(grid.model().horizontal_unit == GridLib::Unit::METER);
    const auto& crs = grid.model().crs;
    REQUIRE(Xyz::are_equal(grid.model().column_axis(), {0, -10, 0}));
    REQUIRE(Xyz::are_equal(grid.model().row_axis(), {10, 0, 0}));
    REQUIRE(Xyz::are_equal(grid.model().vertical_axis(), {0, 0, 1}));
    REQUIRE(crs.code == 25833);
    REQUIRE(crs.type == GridLib::CrsType::PROJECTED);
    REQUIRE(crs.library == GridLib::CrsLibrary::EPSG);
    REQUIRE(crs.citation.starts_with("ESRI PE"));
}
