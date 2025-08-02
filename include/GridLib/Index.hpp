//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-08-02.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Size.hpp"

namespace GridLib
{
    struct Index
    {
        size_t row = 0;
        size_t col = 0;
    };

    inline bool operator==(const Index& a, const Index& b)
    {
        return a.row == b.row && a.col == b.col;
    }

    inline bool operator!=(const Index& a, const Index& b)
    {
        return !(a == b);
    }

    inline Index operator+(const Index& a, const Size& b)
    {
        return {a.row + b.rows, a.col + b.cols};
    }

    inline Index operator-(const Index& a, const Size& b)
    {
        return {a.row - b.rows, a.col - b.cols};
    }

    inline Size operator-(const Index& a, const Index& b)
    {
        return {a.row - b.row, a.col - b.col};
    }
}
