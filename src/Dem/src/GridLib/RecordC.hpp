//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-09-30.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <iosfwd>
#include <optional>

namespace GridLib
{
    struct RecordC
    {
        std::optional<int16_t> has_datum_rmse;
        std::optional<int16_t> datum_rmse[3];
        std::optional<int16_t> datum_rmse_sample_size;
        std::optional<int16_t> has_dem_rmse;
        std::optional<int16_t> dem_rmse[3];
        std::optional<int16_t> dem_rmse_sample_size;
    };

    class FortranReader;

    [[nodiscard]]
    RecordC read_record_c(FortranReader& reader);

    void print(const RecordC& rec, std::ostream& os);
}
