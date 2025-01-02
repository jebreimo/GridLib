//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-10-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "RecordB.hpp"

#include "FortranReader.hpp"

namespace GridLib
{
    constexpr size_t BLOCK_SIZE = 1024;
    RecordB read_record_b(FortranReader& reader)
    {
        RecordB result;
        result.row = *reader.read_int16(6);
        result.column = *reader.read_int16(6);
        result.rows = *reader.read_int16(6);
        result.columns = *reader.read_int16(6);
        result.x = *reader.read_float64(24);
        result.y = *reader.read_float64(24);
        result.elevation_base = *reader.read_float64(24);
        result.elevation_min = reader.read_float64(24);
        result.elevation_max = reader.read_float64(24);

        size_t blockPos = 4 * 6 + 5 * 24;
        size_t remainder = result.rows * result.columns;
        result.elevations.reserve(remainder);
        while (remainder > 0)
        {
            auto n = std::min((BLOCK_SIZE - blockPos) / 6, remainder);
            for (size_t i = 0; i < n; ++i)
                result.elevations.push_back(*reader.read_int16(6));
            remainder -= n;
            blockPos += n * 6;
            reader.skip(BLOCK_SIZE - blockPos);
            blockPos = 0;
        }
        return result;
    }
}
