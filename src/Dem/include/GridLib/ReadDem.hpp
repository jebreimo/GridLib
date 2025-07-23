//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>

#include "GridLib/Grid.hpp"

namespace GridLib
{
    Grid read_dem(std::istream& stream);

    Grid read_dem(const void* buffer, size_t size);

    Grid read_dem(const std::filesystem::path& filename);

    bool is_dem(const std::filesystem::path& filename);
}
