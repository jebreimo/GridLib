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
            result[i] = Yson::get<T>(reader);
        }
        if (reader.nextValue())
            GRIDLIB_THROW("vector contains too many values.");
        reader.leave();
        return result;
    }

    Axis readAxis(Yson::Reader& reader)
    {
        using Yson::get;
        Axis axis;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "unit")
            {
                axis.unit = parseUnit(get<std::string>(reader))
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
        using Yson::get;
        PlanarCoords coords;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "northing")
                coords.northing = get<double>(reader);
            else if (key == "easting")
                coords.easting = get<double>(reader);
            else if (key == "zone")
                coords.zone = get<int>(reader);
        }
        return coords;
    }

    std::optional<SphericalCoords> readSphericalCoords(Yson::Reader& reader)
    {
        using Yson::get;
        SphericalCoords coords;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "latitude")
                coords.latitude = get<double>(reader);
            else if (key == "longitude")
                coords.longitude = get<double>(reader);
        }
        return coords;
    }

    ReferenceSystem readReferenceSystem(Yson::Reader& reader)
    {
        using Yson::get;
        ReferenceSystem system;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "horizontal_system")
                system.horizontal = get<int>(reader);
            else if (key == "vertical_system")
                system.vertical = get<int>(reader);
        }
        return system;
    }

    void readMetadata(Yson::Reader& reader, Grid& grid)
    {
        using Yson::get;
        size_t rowCount = 0;
        size_t columnCount = 0;
        for (const auto& key : Yson::keys(reader))
        {
            if (key == "row_count")
                rowCount = get<uint32_t>(reader);
            else if (key == "column_count")
                columnCount = get<uint32_t>(reader);
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
        using Yson::get;
        auto array = grid.elevations().array();
        size_t index = 0;
        for (Yson::ArrayIterator rowIt(reader); rowIt.next();)
        {
            for (Yson::ArrayIterator colIt(reader); colIt.next();)
            {
                if (reader.isNull())
                {
                    if (!grid.unknownElevation())
                        grid.setUnknownElevation(DBL_TRUE_MIN);
                    array[index++] = DBL_TRUE_MIN;
                }
                else
                {
                    array[index++] = get<double>(reader);
                }
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

    std::string toString(GridFileType type)
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
            return readDem(stream, Unit::UNDEFINED);
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
