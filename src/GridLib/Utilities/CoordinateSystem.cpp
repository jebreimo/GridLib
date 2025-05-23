//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-02-15.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "CoordinateSystem.hpp"

namespace GridLib
{
    Unit epsg_unit_to_unit(int epsg)
    {
        switch (epsg)
        {
        case 9001:
            return Unit::METER;
        case 9002:
            return Unit::FOOT;
        case 9003:
            return Unit::US_SURVEY_FOOT;
        default:
            return Unit::UNDEFINED;
        }
    }

    Unit epsg_crs_to_horizontal_unit(int epsg)
    {
        if (25828 <= epsg && epsg <= 25838)
        {
            return Unit::METER;
        }
        switch (epsg)
        {
        case 4258:
            return Unit::DEGREE;
        default:
            break;
        }
        return {};
    }

    Unit epsg_crs_to_vertical_unit(int epsg)
    {
        switch (epsg)
        {
        case 4937:
        case 5941:
            return Unit::METER;
        default:
            break;
        }
        return {};
    }
}
