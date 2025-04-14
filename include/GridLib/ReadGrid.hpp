//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Grid.hpp"

namespace GridLib
{
    Grid read_json_grid(std::istream& stream);

    Grid read_json_grid(const std::string& file_name);

    enum class GridFileType
    {
        UNKNOWN,
        DEM,
        GEOTIFF,
        GRIDLIB_JSON,
        AUTO_DETECT
    };

    std::string to_string(GridFileType type);

    Grid read_grid(std::istream& stream, GridFileType type);

    Grid read_grid(const std::string& file_name,
                   GridFileType type = GridFileType::AUTO_DETECT);
}
