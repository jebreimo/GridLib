//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <stdexcept>

namespace GridLib
{
    class GridLibException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#define GRIDLIB_EX_3_(file, line, msg) \
    ::GridLib::GridLibException(file ":" #line ": " msg)

#define GRIDLIB_EX_2_(file, line, msg) \
    GRIDLIB_EX_3_(file, line, msg)

#define GRIDLIB_EXCEPTION(msg) \
    GRIDLIB_EX_2_(__FILE__, __LINE__, msg)

#define GRIDLIB_THROW(msg) \
    throw GRIDLIB_EX_2_(__FILE__, __LINE__, msg)
