//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/GridMemberTypes.hpp"

namespace GridLib
{
    #define TO_STRING(name) \
        case Unit::name: return #name

    std::string_view to_string(Unit unit)
    {
        switch (unit)
        {
        TO_STRING(FEET);
        TO_STRING(METERS);
        TO_STRING(ARC_SECONDS);
        default:
            return "UNDEFINED";
        }
    }

    #define CHECK_STRING(str, name) \
        if (str == #name) \
            return Unit::name

    std::optional<Unit> parse_unit(std::string_view str)
    {
        CHECK_STRING(str, METERS);
        CHECK_STRING(str, FEET);
        CHECK_STRING(str, ARC_SECONDS);
        CHECK_STRING(str, UNDEFINED);
        return {};
    }

    double to_meters(Unit unit)
    {
        switch (unit)
        {
        case Unit::FEET:
            return 12 * 0.0254;
        case Unit::ARC_SECONDS:
            return 30.87; // At equator
        case Unit::METERS:
        case Unit::UNDEFINED:
            return 1;
        }
    }
}
