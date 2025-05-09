//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-12-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "IGrid.hpp"

namespace Yson
{
    class Writer;
}

namespace GridLib
{
    void write_json(std::ostream& stream, const IGrid& grid);

    void write_json(const std::string& file_name, const IGrid& grid);

    void write_json(Yson::Writer& writer, const IGrid& grid);
}
