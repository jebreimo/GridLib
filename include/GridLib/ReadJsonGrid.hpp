//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-07-06.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>

#include "Grid.hpp"

namespace GridLib
{
    Grid read_json_grid(std::istream& stream, bool strict = false);

    Grid read_json_grid(const std::filesystem::path& filename, bool strict = false);

    Grid read_json_grid(const void* buffer, size_t size, bool strict = false);
}
