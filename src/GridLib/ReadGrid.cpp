//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadGrid.hpp"

#include <map>
#include <Yson/ReaderIterators.hpp>
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

    Axis read_axis(Yson::Reader& reader)
    {
        using Yson::read;
        Axis axis;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "unit")
            {
                axis.unit = parse_unit(read<std::string>(reader))
                    .value_or(Unit::UNDEFINED);
            }
            else if (key == "direction")
            {
                axis.direction = read_vector<double, 3>(reader);
            }
        }
        return axis;
    }

    PlanarCoords read_planar_coords(Yson::Reader& reader)
    {
        using Yson::read;
        PlanarCoords coords;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "northing")
                coords.northing = read<double>(reader);
            else if (key == "easting")
                coords.easting = read<double>(reader);
            else if (key == "elevation")
                coords.elevation = read<double>(reader);
            else if (key == "zone")
                coords.zone = read<int>(reader);
        }
        return coords;
    }

    std::optional<SphericalCoords> read_spherical_coords(Yson::Reader& reader)
    {
        using Yson::read;
        SphericalCoords coords;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "latitude")
                coords.latitude = read<double>(reader);
            else if (key == "longitude")
                coords.longitude = read<double>(reader);
        }
        return coords;
    }

    ReferenceSystem read_reference_system(Yson::Reader& reader)
    {
        using Yson::read;
        ReferenceSystem system;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "projected")
                system.projected = read<int>(reader);
            else if (key == "vertical")
                system.vertical = read<int>(reader);
        }
        return system;
    }

    void read_metadata(Yson::Reader& reader, Grid& grid)
    {
        using Yson::read;
        size_t row_count = 0;
        size_t column_count = 0;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "row_count")
                row_count = read<uint32_t>(reader);
            else if (key == "column_count")
                column_count = read<uint32_t>(reader);
            else if (key == "row_axis")
                grid.set_row_axis(read_axis(reader));
            else if (key == "column_axis")
                grid.set_column_axis(read_axis(reader));
            else if (key == "vertical_axis")
                grid.set_vertical_axis(read_axis(reader));
            else if (key == "planar_coords")
                grid.set_planar_coords(read_planar_coords(reader));
            else if (key == "spherical_coords")
                grid.set_spherical_coords(read_spherical_coords(reader));
            else if (key == "reference_system")
                grid.set_reference_system(read_reference_system(reader));
        }
        grid.resize(row_count, column_count);
    }

    void read_elevations(Yson::Reader& reader, Grid& grid)
    {
        using Yson::read;
        auto array = grid.elevations().array();
        size_t index = 0;
        for (Yson::ArrayIterator row_it(reader); row_it.next();)
        {
            for (Yson::ArrayIterator col_it(reader); col_it.next();)
            {
                if (reader.readNull())
                {
                    if (!grid.unknown_elevation())
                        grid.set_unknown_elevation(FLT_TRUE_MIN);
                    array[index++] = FLT_TRUE_MIN;
                }
                else
                {
                    array[index++] = read<float>(reader);
                }
            }
        }
    }

    Grid read_grid(Yson::Reader& reader)
    {
        Grid grid;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "metadata")
                read_metadata(reader, grid);
            else if (key == "elevations")
                read_elevations(reader, grid);
        }
        return grid;
    }

    Grid read_json_grid(std::istream& stream)
    {
        return read_grid(*Yson::makeReader(stream));
    }

    Grid read_json_grid(const std::string& file_name)
    {
        return read_grid(*Yson::makeReader(file_name));
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
            return read_dem(stream, Unit::UNDEFINED);
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
        if (is_geotiff(fileName))
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
            return read_dem(file_name, Unit::METERS);
#endif
#ifdef GridLib_GEOTIFF_SUPPORT
        case GridFileType::GEOTIFF:
            return read_geotiff(file_name);
#endif
        default:
            GRIDLIB_THROW("Unsupported file type: " + file_name);
        }
    }
}
