//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-01-22.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadGrid.hpp"

#include <map>
#include <Yson/Reader.hpp>
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    Axis readAxis(Yson::Reader& reader)
    {
        using Yson::read;
        Axis axis;
        reader.enter();
        while (reader.nextKey())
        {
            auto key = read<std::string>(reader);
            reader.nextValue();
            if (key == "unit")
            {
                axis.unit = parseUnit(read<std::string>(reader))
                    .value_or(Unit::UNDEFINED);
            }
            else if (key == "resolution")
            {
                axis.resolution = read<double>(reader);
            }
        }
        reader.leave();
        return axis;
    }

    PlanarCoords readPlanarCoords(Yson::Reader& reader)
    {
        using Yson::read;
        PlanarCoords coords;
        reader.enter();
        while (reader.nextKey())
        {
            auto key = read<std::string>(reader);
            reader.nextValue();
            if (key == "northing")
                coords.northing = read<double>(reader);
            else if (key == "easting")
                coords.easting = read<double>(reader);
            else if (key == "zone")
                coords.zone = read<int>(reader);
        }
        reader.leave();
        return coords;
    }

    std::optional<SphericalCoords> readSphericalCoords(Yson::Reader& reader)
    {
        using Yson::read;
        SphericalCoords coords;
        reader.enter();
        while (reader.nextKey())
        {
            auto key = read<std::string>(reader);
            reader.nextValue();
            if (key == "latitude")
                coords.latitude = read<double>(reader);
            else if (key == "longitude")
                coords.longitude = read<double>(reader);
        }
        reader.leave();
        return coords;
    }

    ReferenceSystem readReferenceSystem(Yson::Reader& reader)
    {
        using Yson::read;
        ReferenceSystem system;
        reader.enter();
        while (reader.nextKey())
        {
            auto key = read<std::string>(reader);
            reader.nextValue();
            if (key == "horizontal_system")
                system.horizontal = read<int>(reader);
            else if (key == "vertical_system")
                system.vertical = read<int>(reader);
        }
        reader.leave();
        return system;
    }

    void readMetadata(Yson::Reader& reader, Grid& grid)
    {
        using Yson::read;
        size_t rowCount = 0;
        size_t columnCount = 0;
        reader.enter();
        while (reader.nextKey())
        {
            auto key = read<std::string>(reader);
            reader.nextValue();
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
            else if (key == "rotation_angle")
                grid.setRotationAngle(read<double>(reader));
            else if (key == "planar_coords")
                grid.setPlanarCoords(readPlanarCoords(reader));
            else if (key == "spherical_coords")
                grid.setSphericalCoords(readSphericalCoords(reader));
            else if (key == "reference_system")
                grid.setReferenceSystem(readReferenceSystem(reader));
        }
        reader.leave();
        grid.resize(rowCount, columnCount);
    }

    void readElevations(Yson::Reader& reader, Grid& grid)
    {
        using Yson::read;
        auto array = grid.elevations().array();
        size_t index = 0;
        reader.enter();
        while (reader.nextValue())
        {
            reader.enter();
            while (reader.nextValue())
            {
                if (index == array.size())
                    GRIDLIB_THROW("Too many elevations.");
                array[index++] = read<double>(reader);
            }
            reader.leave();
        }
        reader.leave();
    }

    Grid readGrid(Yson::Reader& reader)
    {
        Grid grid;
        reader.nextValue();
        reader.enter();
        while (reader.nextKey())
        {
            auto key = Yson::read<std::string>(reader);
            reader.nextValue();
            if (key == "metadata")
                readMetadata(reader, grid);
            else if (key == "elevations")
                readElevations(reader, grid);
        }
        reader.leave();
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
