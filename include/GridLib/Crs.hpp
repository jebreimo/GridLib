//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-09.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>

namespace GridLib
{
    enum class CrsType
    {
        UNKNOWN,
        PROJECTED,
        GEOGRAPHIC
    };

    std::string to_string(CrsType type);

    CrsType parse_crs_type(const std::string& str);

    enum class CrsLibrary
    {
        UNKNOWN,
        EPSG
    };

    std::string to_string(CrsLibrary type);

    CrsLibrary parse_crs_library(const std::string& str);

    struct Crs
    {
        int code = 0;
        int vertical_code = 0;
        CrsType type = CrsType::UNKNOWN;
        CrsLibrary library = CrsLibrary::UNKNOWN;
        std::string citation;

        explicit operator bool() const
        {
            return code != 0
                   || vertical_code != 0
                   || !citation.empty();
        }
    };

    bool operator==(const Crs& lhs, const Crs& rhs);
}
