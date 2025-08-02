//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-08-02.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

namespace GridLib
{
    struct Size
    {
        size_t rows = 0;
        size_t cols = 0;
    };

    inline bool operator==(const Size& a, const Size& b)
    {
        return a.rows == b.rows && a.cols == b.cols;
    }

    inline bool operator!=(const Size& a, const Size& b)
    {
        return !(a == b);
    }

    inline Size operator+(const Size& a, const Size& b)
    {
        return {a.rows + b.rows, a.cols + b.cols};
    }

    inline Size operator-(const Size& a, const Size& b)
    {
        return {a.rows - b.rows, a.cols - b.cols};
    }

    inline Size operator*(const Size& a, const Size& b)
    {
        return {a.rows * b.rows, a.cols * b.cols};
    }

    inline Size operator/(const Size& a, const Size& b)
    {
        return {a.rows / b.rows, a.cols / b.cols};
    }

    inline Size operator%(const Size& a, const Size& b)
    {
        return {a.rows % b.rows, a.cols % b.cols};
    }
}
