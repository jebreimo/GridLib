//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-20.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Unit.hpp"

namespace GridLib
{
    #define TO_STRING(name) \
        case Unit::name: return #name

    std::string_view to_string(Unit unit)
    {
        switch (unit)
        {
        TO_STRING(METER);
        TO_STRING(FOOT);
        TO_STRING(US_SURVEY_FOOT);
        TO_STRING(DEGREE);
        TO_STRING(ARC_SECOND);
        default:
            return "UNDEFINED";
        }
    }

    #define CHECK_STRING(str, name) \
        if (str == #name) \
            return Unit::name

    std::optional<Unit> parse_unit(std::string_view str)
    {
        CHECK_STRING(str, METER);
        CHECK_STRING(str, FOOT);
        CHECK_STRING(str, US_SURVEY_FOOT);
        CHECK_STRING(str, DEGREE);
        CHECK_STRING(str, ARC_SECOND);
        CHECK_STRING(str, UNDEFINED);
        return {};
    }

    double to_meters(Unit unit)
    {
        switch (unit)
        {
        case Unit::UNDEFINED:
        case Unit::METER:
        default:
            return 1;
        case Unit::FOOT:
            return 12 * 0.0254;
        case Unit::US_SURVEY_FOOT:
            return 1200.0 / 3937.0;
        }
    }
}
