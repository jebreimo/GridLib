//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <stdexcept>

namespace GridLib
{
    class GridLibException : public std::runtime_error
    {
    public:
        explicit GridLibException(const std::string& message) noexcept
            : std::runtime_error(message)
        {}
    };
}

#define GRIDLIB_THROW_3_(file, line, msg) \
    throw ::GridLib::GridLibException(file ":" #line ": " msg)

#define GRIDLIB_THROW_2_(file, line, msg) \
    GRIDLIB_THROW_3_(file, line, msg)

#define GRIDLIB_THROW(msg) \
    GRIDLIB_THROW_2_(__FILE__, __LINE__, msg)
