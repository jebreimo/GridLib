//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadGrid.hpp"

#include <map>
#include <Yson/ReaderIterators.hpp>

#include "GridBuilder.hpp"
#include "GridLib/GridLibException.hpp"
#include "GridLib/ReadJsonGrid.hpp"
#include "GridLibVersion.hpp"

#ifdef GridLib_DEM_SUPPORT
#include "GridLib/ReadDem.hpp"
#endif

#ifdef GridLib_GEOTIFF_SUPPORT
#include "GridLib/ReadGeoTiff.hpp"
#endif

namespace GridLib
{
#define CASE_ENUM(type, value) \
        case type::value: return #value;

    std::string to_string(GridFileType type)
    {
        switch (type)
        {
        CASE_ENUM(GridFileType, UNKNOWN);
        CASE_ENUM(GridFileType, GRIDLIB_JSON);
        CASE_ENUM(GridFileType, DEM);
        CASE_ENUM(GridFileType, AUTO_DETECT);
        default:
            GRIDLIB_THROW("Unknown GridFileType: "
                + std::to_string(int(type)));
        }
    }

    Grid read_grid(std::istream& stream, GridFileType type)
    {
        switch (type)
        {
        case GridFileType::GRIDLIB_JSON:
            return read_json_grid(stream);
#ifdef GridLib_DEM_SUPPORT
        case GridFileType::DEM:
            return read_dem(stream);
#endif
#ifdef GridLib_GEOTIFF_SUPPORT
        case GridFileType::GEOTIFF:
            return read_geotiff(stream);
#endif
        default:
            GRIDLIB_THROW("Can not read stream of type " + to_string(type));
        }
    }

    GridFileType detect_file_type(const std::filesystem::path& fileName)
    {
#ifdef GridLib_DEM_SUPPORT
        if (is_dem(fileName))
            return GridFileType::DEM;
#endif
#ifdef GridLib_GEOTIFF_SUPPORT
        if (is_tiff(fileName))
            return GridFileType::GEOTIFF;
#endif
        if (Yson::makeReader(fileName))
            return GridFileType::GRIDLIB_JSON;
        return GridFileType::UNKNOWN;
    }

    Grid read_grid(const std::filesystem::path& filename, GridFileType type)
    {
        if (type == GridFileType::AUTO_DETECT)
            type = detect_file_type(filename);

        switch (type)
        {
        case GridFileType::GRIDLIB_JSON:
            return read_json_grid(filename);
#ifdef GridLib_DEM_SUPPORT
        case GridFileType::DEM:
            return read_dem(filename);
#endif
#ifdef GridLib_GEOTIFF_SUPPORT
        case GridFileType::GEOTIFF:
            return read_geotiff(filename);
#endif
        default:
            GRIDLIB_THROW("Unsupported file type: " + filename.string());
        }
    }

    Grid read_grid(const void* buffer, size_t size, GridFileType type)
    {
        switch (type)
        {
        case GridFileType::GRIDLIB_JSON:
            return read_json_grid(buffer, size);
#ifdef GridLib_DEM_SUPPORT
        case GridFileType::DEM:
            return read_dem(buffer, size);
#endif
#ifdef GridLib_GEOTIFF_SUPPORT
        case GridFileType::GEOTIFF:
            return read_geotiff(buffer, size);
#endif
        default:
            GRIDLIB_THROW("Unsupported file type: " + to_string(type));
        }
    }
}
