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

#ifdef GRIDLIB_DEM_SUPPORT
    #include "GridLib/ReadDem.hpp"
#endif

namespace GridLib
{
    template <typename T, size_t N>
    Xyz::Vector<T, N> readVector(Yson::Reader& reader)
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

    Axis readAxis(Yson::Reader& reader)
    {
        using Yson::read;
        Axis axis;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "unit")
            {
                axis.unit = parseUnit(read<std::string>(reader))
                    .value_or(Unit::UNDEFINED);
            }
            else if (key == "direction")
            {
                axis.direction = readVector<double, 3>(reader);
            }
        }
        return axis;
    }

    PlanarCoords readPlanarCoords(Yson::Reader& reader)
    {
        using Yson::read;
        PlanarCoords coords;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "northing")
                coords.northing = read<double>(reader);
            else if (key == "easting")
                coords.easting = read<double>(reader);
            else if (key == "zone")
                coords.zone = read<int>(reader);
        }
        return coords;
    }

    std::optional<SphericalCoords> readSphericalCoords(Yson::Reader& reader)
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

    ReferenceSystem readReferenceSystem(Yson::Reader& reader)
    {
        using Yson::read;
        ReferenceSystem system;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "horizontal_system")
                system.horizontal = read<int>(reader);
            else if (key == "vertical_system")
                system.vertical = read<int>(reader);
        }
        return system;
    }

    void readMetadata(Yson::Reader& reader, Grid& grid)
    {
        using Yson::read;
        size_t rowCount = 0;
        size_t columnCount = 0;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "row_count")
                rowCount = read<uint32_t>(reader);
            else if (key == "column_count")
                columnCount = read<uint32_t>(reader);
            else if (key == "row_axis")
                grid.setRowAxis(readAxis(reader));
            else if (key == "column_axis")
                grid.setColumnAxis(readAxis(reader));
            else if (key == "vertical_axis")
                grid.setVerticalAxis(readAxis(reader));
            else if (key == "planar_coords")
                grid.setPlanarCoords(readPlanarCoords(reader));
            else if (key == "spherical_coords")
                grid.setSphericalCoords(readSphericalCoords(reader));
            else if (key == "reference_system")
                grid.setReferenceSystem(readReferenceSystem(reader));
        }
        grid.resize(rowCount, columnCount);
    }

    void readElevations(Yson::Reader& reader, Grid& grid)
    {
        using Yson::read;
        auto array = grid.elevations().array();
        size_t index = 0;
        for (Yson::ArrayIterator it(reader); it.next();)
        {
            for (const auto& value : Yson::arrayValues<double>(reader))
            {
                if (index == array.size())
                    GRIDLIB_THROW("Too many elevations.");
                array[index++] = value;
            }
        }
    }

    Grid readGrid(Yson::Reader& reader)
    {
        Grid grid;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "metadata")
                readMetadata(reader, grid);
            else if (key == "elevations")
                readElevations(reader, grid);
        }
        return grid;
    }

    Grid readJsonGrid(std::istream& stream)
    {
        return readGrid(*Yson::makeReader(stream));
    }

    Grid readJsonGrid(const std::string& fileName)
    {
        return readGrid(*Yson::makeReader(fileName));
    }

    #define CASE_ENUM(type, value) \
        case type::value: return #value;

    std::string GridLib::toString(GridFileType type)
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

    Grid readGrid(std::istream& stream, GridFileType type)
    {
        switch (type)
        {
        case GridFileType::GRIDLIB_JSON:
            return readJsonGrid(stream);
#ifdef GRIDLIB_DEM_SUPPORT
        case GridFileType::DEM:
            return readDem(stream, Unit::METERS);
#endif
        default:
            GRIDLIB_THROW("Can not read stream of type " + toString(type));
        }
        return Grid();
    }

    GridFileType detectFileType(const std::string& fileName)
    {
#ifdef GRIDLIB_DEM_SUPPORT
        if (isDem(fileName))
            return GridFileType::DEM;
#endif
        if (Yson::makeReader(fileName))
            return GridFileType::GRIDLIB_JSON;
        return GridFileType::UNKNOWN;
    }

    Grid readGrid(const std::string& fileName, GridFileType type)
    {
        if (type == GridFileType::AUTO_DETECT)
            type = detectFileType(fileName);

        switch (type)
        {
        case GridFileType::GRIDLIB_JSON:
            return readJsonGrid(fileName);
#ifdef GRIDLIB_DEM_SUPPORT
        case GridFileType::DEM:
            return readDem(fileName, Unit::METERS);
#endif
        default:
            GRIDLIB_THROW("Unsupported file type: " + fileName);
        }
    }
}
