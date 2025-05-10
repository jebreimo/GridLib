//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Crs.hpp"

#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    #define TO_STRING(enum_type, name) \
        case enum_type::name: return #name

    #define CHECK_STRING(enum_type, str, name) \
        if ((str) == #name) \
            return enum_type::name

    std::string to_string(CrsType type)
    {
        switch (type)
        {
            TO_STRING(CrsType, PROJECTED);
            TO_STRING(CrsType, GEOGRAPHIC);
            default:
                return "UNKNOWN";
        }
    }

    CrsType parse_crs_type(const std::string& str)
    {
        CHECK_STRING(CrsType, str, PROJECTED);
        CHECK_STRING(CrsType, str, GEOGRAPHIC);
        CHECK_STRING(CrsType, str, UNKNOWN);
        GRIDLIB_THROW("Unknown crs type: " + str);
    }

    std::string to_string(CrsLibrary type)
    {
        switch (type)
        {
            TO_STRING(CrsLibrary, EPSG);
            default:
                return "UNKNOWN";
        }
    }

    CrsLibrary parse_crs_library(const std::string& str)
    {
        CHECK_STRING(CrsLibrary, str, EPSG);
        CHECK_STRING(CrsLibrary, str, UNKNOWN);
        GRIDLIB_THROW("Unknown crs library: " + str);
    }

    bool operator==(const Crs& lhs, const Crs& rhs)
    {
        return lhs.type == rhs.type
               && lhs.library == rhs.library
               && lhs.code == rhs.code
               && lhs.vertical_code == rhs.vertical_code
               && lhs.citation == rhs.citation;
    }
}
