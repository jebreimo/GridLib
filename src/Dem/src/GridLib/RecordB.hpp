//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-09-30.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace GridLib
{
    struct RecordB
    {
        int16_t row;
        int16_t column;
        int16_t rows;
        int16_t columns;
        double x;
        double y;
        double elevation_base;
        std::optional<double> elevation_min;
        std::optional<double> elevation_max;
        std::vector<int32_t> elevations;
    };

    class FortranReader;

    [[nodiscard]]
    RecordB read_record_b(FortranReader& reader);
}
