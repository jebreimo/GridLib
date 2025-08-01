//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-07-06.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadJsonGrid.hpp"

#include <cmath>
#include <map>
#include <Yson/ReaderIterators.hpp>

#include "GridBuilder.hpp"
#include "GridLib/GridLibException.hpp"

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
            for (unsigned i = 0; i < N; ++i)
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
        return parse_unit(read<std::string>(reader));
    }

    Crs read_crs(Yson::Reader& reader, bool strict)
    {
        Crs result;
        for (const auto& key : keys(reader))
        {
            if (key == "code")
                result.code = read<int>(reader);
            else if (key == "vertical_code")
                result.vertical_code = read<int>(reader);
            else if (key == "type")
                result.type = parse_crs_type(read<std::string>(reader));
            else if (key == "library")
                result.library = parse_crs_library(read<std::string>(reader));
            else if (key == "citation")
                result.citation = read<std::string>(reader);
            else if (strict)
                GRIDLIB_THROW("Unknown key: '" + key + "'" + get_reader_position(reader));
        }
        return result;
    }

    std::vector<std::pair<std::string, std::string>>
    read_dictionary(Yson::Reader& reader)
    {
        std::vector<std::pair<std::string, std::string>> result;
        for (const auto& key : keys(reader))
            result.emplace_back(key, read<std::string>(reader));
        return result;
    }

    SpatialData read_model(Yson::Reader& reader, bool strict)
    {
        using Yson::read;
        SpatialData result;
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
                result.crs = read_crs(reader, strict);
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
                    point.crs = read_crs(reader, strict);
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
                builder.values = read_elevations(reader);
            else if (strict)
                GRIDLIB_THROW("Unknown key: '" + key + "'" + get_reader_position(reader));
        }
        return builder.build();
    }

    Grid read_json_grid(std::istream& stream, bool strict)
    {
        return read_grid(*Yson::makeReader(stream), strict);
    }

    Grid read_json_grid(const std::filesystem::path& filename, bool strict)
    {
        return read_grid(*Yson::makeReader(filename), strict);
    }

    Grid read_json_grid(const void* buffer, size_t size, bool strict)
    {
        auto str = static_cast<const char*>(buffer);
        return read_grid(*Yson::makeReader(str, size), strict);
    }
}
