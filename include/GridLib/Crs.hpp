//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <variant>

namespace GridLib
{
    struct ProjectedCrs
    {
        int projection = 0;
        int vertical = 0;

        explicit operator bool() const
        {
            return projection != 0 || vertical != 0;
        }
    };

    inline bool operator==(const ProjectedCrs& a, const ProjectedCrs& b)
    {
        return a.projection == b.projection
               && a.vertical == b.vertical;
    }

    struct GeographicCrs
    {
        int geographic = 0;
        int vertical = 0;

        explicit operator bool() const
        {
            return geographic != 0 || vertical != 0;
        }
    };

    inline bool operator==(const GeographicCrs& a, const GeographicCrs& b)
    {
        return a.geographic == b.geographic
               && a.vertical == b.vertical;
    }

    using Crs = std::variant<std::monostate, ProjectedCrs, GeographicCrs>;
}
