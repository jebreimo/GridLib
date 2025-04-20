//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <functional>
#include "GridLib/Grid.hpp"

namespace GridLib
{
    using ProgressCallback = std::function<bool (size_t, size_t)>;

    Grid read_dem(std::istream& stream,
                  const ProgressCallback& progress_callback = {});

    Grid read_dem(const std::string& file_name,
                  const ProgressCallback& progress_callback = {});

    bool is_dem(const std::string& file_name);
}
