//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GridView.hpp"

namespace GridLib
{
    void writeJsonGrid(std::ostream& stream, const GridView& grid);

    void writeJsonGrid(const std::string& fileName, const GridView& grid);
}
