//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-01.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../../include/GridLib/GridRect.hpp"

#include <algorithm>
#include <climits>

namespace GridLib
{
    GridPos add(const GridPos& pos, const GridSize& size)
    {
        auto r = pos.row + size.rows;
        auto c = pos.column + size.columns;
        if (r < pos.row)
            r = UINT_MAX;
        if (c < pos.column)
            c = UINT_MAX;
        return {r, c};
    }

    GridRect get_intersection(const GridRect& a, const GridRect& b)
    {
        auto a_max = add(a.pos, a.size);
        auto b_max = add(b.pos, b.size);
        auto r_min = std::max(a.pos.row, b.pos.row);
        auto r_max = std::min(a_max.row, b_max.row);
        auto c_min = std::max(a.pos.column, b.pos.column);
        auto c_max = std::min(a_max.column, b_max.column);
        if (r_min <= r_max && c_min <= c_max)
            return {{r_min, c_min}, {r_max - r_min, c_max - c_min}};
        else
            return {};
    }

    bool is_empty(const GridSize& size)
    {
        return size.rows == 0 || size.columns == 0;
    }

    bool is_empty(const GridRect& rect)
    {
        return is_empty(rect.size);
    }
}
