//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/GridModel.hpp"

namespace GridLib
{
    bool operator==(const GridModel& lhs, const GridModel& rhs)
    {
        return lhs.matrix == rhs.matrix
               && lhs.horizontal_unit == rhs.horizontal_unit
               && lhs.vertical_unit == rhs.vertical_unit
               && lhs.reference_system == rhs.reference_system
               && lhs.unknown_elevation == rhs.unknown_elevation;
    }

    bool operator!=(const GridModel& lhs, const GridModel& rhs)
    {
        return !(lhs == rhs);
    }
}
