//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-04-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <optional>
#include <string_view>

namespace GridLib
{
    enum class Unit
    {
        UNDEFINED = 0,
        METER = 1,
        FOOT = 2,
        US_SURVEY_FOOT = 3,
        DEGREE = 4,
        ARC_SECOND = 5
    };

    std::string_view to_string(Unit unit);

    Unit parse_unit(std::string_view str);

    std::optional<Unit> try_parse_unit(std::string_view str);

    double to_meters(Unit unit);
}
