//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-01.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace GridLib
{
    struct GridPos
    {
        unsigned row;
        unsigned column;
    };

    constexpr bool operator==(const GridPos& a, const GridPos& b)
    {
        return a.row == b.row && a.column == b.column;
    }

    struct GridSize
    {
        unsigned rows;
        unsigned columns;
    };

    constexpr bool operator==(const GridSize& a, const GridSize& b)
    {
        return a.rows == b.rows && a.columns == b.columns;
    }

    bool is_empty(const GridSize& size);

    GridPos add(const GridPos& pos, const GridSize& size);

    struct GridRect
    {
        GridPos pos;
        GridSize size;
    };

    constexpr bool operator==(const GridRect& a, const GridRect& b)
    {
        return a.pos == b.pos && a.size == b.size;
    }

    GridRect get_intersection(const GridRect& a, const GridRect& b);

    bool is_empty(const GridRect& rect);
}
