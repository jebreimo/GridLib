//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GridLib/Grid.hpp"

namespace GridLib
{
    [[nodiscard]] Grid read_geotiff(std::istream& stream);

    [[nodiscard]] Grid read_geotiff(const std::string& file_name);

    [[nodiscard]] bool is_geotiff(const std::string& file_name);
}
