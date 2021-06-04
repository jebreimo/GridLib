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
}
