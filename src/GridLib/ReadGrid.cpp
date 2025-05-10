//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadGrid.hpp"

#include <cmath>
#include <map>
#include <Xyz/Matrix.hpp>
#include <Yson/ReaderIterators.hpp>

#include "GridBuilder.hpp"
#include "GridLib/GridLibException.hpp"
#include "GridLibVersion.hpp"

#ifdef GridLib_DEM_SUPPORT
#include "GridLib/ReadDem.hpp"
#endif

#ifdef GridLib_GEOTIFF_SUPPORT
#include "GridLib/ReadGeoTiff.hpp"
#endif

namespace GridLib
{
    namespace
    {
        std::string get_reader_position(Yson::Reader& reader)
        {
            return " [line " + std::to_string(reader.lineNumber())
                   + ", column " + std::to_string(reader.columnNumber())
                   + "]";
        }

        template <typename T, size_t N>
        Xyz::Vector<T, N> read_vector(Yson::Reader& reader)
        {
            Xyz::Vector<T, N> result;
            reader.enter();
            for (size_t i = 0; i < N; ++i)
            {
                reader.nextValue();
                result[i] = Yson::read<T>(reader);
            }
            if (reader.nextValue())
                GRIDLIB_THROW("vector contains too many values.");
            reader.leave();
            return result;
        }
    }

    Unit read_unit(Yson::Reader& reader)
    {
        return parse_unit(read<std::string>(reader)).value_or(Unit::UNDEFINED);
    }

    Crs read_crs(Yson::Reader& reader)
    {
        auto values = reader.readItem();
        auto type = Yson::get<std::string>(values["type"]);
        if (type == "projection")
        {
            return ProjectedCrs
            {
                Yson::get<int>(values["projection"]),
                Yson::get<int>(values["vertical"]),
            };
        }

        if (type == "geographic")
        {
            return GeographicCrs
            {
                Yson::get<int>(values["geographic"]),
                Yson::get<int>(values["vertical"]),
            };
        }

        return {};
    }

    std::vector<std::pair<std::string, std::string>>
    read_dictionary(Yson::Reader& reader)
    {
        std::vector<std::pair<std::string, std::string>> result;
        for (const auto& key : keys(reader))
            result.emplace_back(key, read<std::string>(reader));
        return result;
    }

    GridModel read_model(Yson::Reader& reader, bool strict)
    {
        using Yson::read;
        GridModel result;
        for (const auto& key : keys(reader))
        {
            if (key == "location")
                result.set_location(read_vector<double, 3>(reader));
            else if (key == "row_axis")
                result.set_row_axis(read_vector<double, 3>(reader));
            else if (key == "column_axis")
                result.set_column_axis(read_vector<double, 3>(reader));
            else if (key == "vertical_axis")
                result.set_vertical_axis(read_vector<double, 3>(reader));
            else if (key == "horizontal_unit")
                result.horizontal_unit = read_unit(reader);
            else if (key == "vertical_unit")
                result.vertical_unit = read_unit(reader);
            else if (key == "crs")
                result.crs = read_crs(reader);
            else if (key == "information")
                result.information = read_dictionary(reader);
            else if (strict)
                GRIDLIB_THROW("Unknown key: '" + key + "'" + get_reader_position(reader));
        }
        return result;
    }

    std::vector<SpatialTiePoint>
    read_spatial_tie_points(Yson::Reader& reader, bool strict)
    {
        std::vector<SpatialTiePoint> result;
        reader.enter();
        while (reader.nextValue())
        {
            using Yson::read;
            SpatialTiePoint point;
            for (const auto& key : keys(reader))
            {
                if (key == "grid_point")
                    point.grid_point = read_vector<double, 2>(reader);
                else if (key == "location")
                    point.location = read_vector<double, 3>(reader);
                else if (key == "crs")
                    point.crs = read_crs(reader);
                else if (strict)
                    GRIDLIB_THROW("Unknown key: '" + key + "'");
            }
            result.push_back(point);
        }
        reader.leave();
        return result;
    }

    std::vector<float> read_elevations(Yson::Reader& reader)
    {
        using Yson::read;
        std::vector<float> result;
        for (Yson::ArrayIterator row_it(reader); row_it.next();)
        {
            for (Yson::ArrayIterator col_it(reader); col_it.next();)
            {
                if (reader.readNull())
                    result.push_back(NAN);
                else
                    result.push_back(read<float>(reader));
            }
        }
        return result;
    }

    Grid read_grid(Yson::Reader& reader, bool strict)
    {
        GridBuilder builder;
        for (const auto& key : keys(reader))
        {
            if (key == "row_count")
                builder.row_count = read<uint32_t>(reader);
            else if (key == "column_count")
                builder.col_count = read<uint32_t>(reader);
            else if (key == "model_tie_point")
                builder.model_tie_point = read_vector<double, 2>(reader);
            else if (key == "model")
                builder.model = read_model(reader, strict);
            else if (key == "spatial_tie_points")
                builder.spatial_tie_points = read_spatial_tie_points(reader, strict);
            else if (key == "elevations")
                builder.elevations = read_elevations(reader);
            else if (strict)
                GRIDLIB_THROW("Unknown key: '" + key + "'" + get_reader_position(reader));
        }
        return builder.build();
    }

    Grid read_json_grid(std::istream& stream, bool strict)
    {
        return read_grid(*Yson::makeReader(stream), strict);
    }

    Grid read_json_grid(const std::string& file_name, bool strict)
    {
        return read_grid(*Yson::makeReader(file_name), strict);
    }

    Grid read_json_grid(const void* buffer, size_t size, bool strict)
    {
        auto str = static_cast<const char*>(buffer);
        return read_grid(*Yson::makeReader(str, size), strict);
    }

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

    GridFileType detect_file_type(const std::string& fileName)
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

    Grid read_grid(const std::string& file_name, GridFileType type)
    {
        if (type == GridFileType::AUTO_DETECT)
            type = detect_file_type(file_name);

        switch (type)
        {
        case GridFileType::GRIDLIB_JSON:
            return read_json_grid(file_name);
#ifdef GridLib_DEM_SUPPORT
        case GridFileType::DEM:
            return read_dem(file_name);
#endif
#ifdef GridLib_GEOTIFF_SUPPORT
        case GridFileType::GEOTIFF:
            return read_geotiff(file_name);
#endif
        default:
            GRIDLIB_THROW("Unsupported file type: " + file_name);
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
