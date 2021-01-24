//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Grid.hpp"

namespace GridLib
{
    Grid readJsonGrid(std::istream& stream);

    Grid readJsonGrid(const std::string& fileName);
}
