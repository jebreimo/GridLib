//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-12-03.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/WriteGrid.hpp"
#include <Yson/JsonWriter.hpp>
#include <fstream>
#include <GridLib/GridLibException.hpp>

namespace GridLib
{
    namespace
    {
        void writeAxis(Yson::Writer& writer, const Axis& axis)
        {
            writer.beginObject();
            writer.key("unit").value(std::string(toString(axis.unit)));
            writer.key("resolution").value(axis.resolution);
            writer.endObject();
        }

        void writeMetadata(Yson::Writer& writer, const GridView& grid)
        {
            writer.beginObject();
            writer.key("row_count").value(uint64_t(grid.rowCount()));
            writer.key("column_count").value(uint64_t(grid.columnCount()));
            writer.key("row_axis");
            writeAxis(writer, grid.rowAxis());
            writer.key("column_axis");
            writeAxis(writer, grid.columnAxis());
            writer.key("vertical_axis");
            writeAxis(writer, grid.verticalAxis());
            writer.key("rotation_angle").value(grid.rotationAngle());

            if (grid.axisOrientation() != RotationDir::COUNTERCLOCKWISE)
                writer.key("axis_orientation").value("CLOCKWISE");

            if (const auto& coords = grid.planarCoords())
            {
                writer.key("planar_coords")
                    .beginObject()
                    .key("northing").value(coords->northing)
                    .key("easting").value(coords->easting)
                    .key("zone").value(coords->zone)
                    .endObject();
            }

            if (const auto& coords = grid.sphericalCoords())
            {
                writer.key("spherical_coords")
                    .beginObject()
                    .key("latitude").value(coords->latitude)
                    .key("longitude").value(coords->longitude)
                    .endObject();
            }

            if (const auto& refSys = grid.referenceSystem())
            {
                writer.key("reference_system")
                    .beginObject()
                    .key("horizontal_system").value(refSys->horizontal);
                if (refSys->vertical)
                    writer.key("vertical_system").value(refSys->vertical);
                writer.endObject();
            }

            writer.endObject();
        }

        void writeElevations(Yson::Writer& writer,
                             const Chorasmia::ArrayView2D<double>& values,
                             std::optional<double> unknownElevation)
        {
            writer.beginArray();
            for (const auto& row : values)
            {
                writer.beginArray(Yson::JsonParameters(10));
                for (const auto& value : row)
                {
                    if (!unknownElevation || value != *unknownElevation)
                        writer.value(value);
                    else
                        writer.null();
                }
                writer.endArray();
            }
            writer.endArray();
        }
    }

    void writeJsonGrid(std::ostream& stream, const GridView& grid)
    {
        Yson::JsonWriter writer(stream, Yson::JsonFormatting::FORMAT);
        writer.beginObject();
        writer.key("metadata");
        writeMetadata(writer, grid);
        writer.key("elevations");
        writeElevations(writer, grid.elevations(), grid.unknownElevation());
        writer.endObject();
    }

    void writeJsonGrid(const std::string& fileName, const GridView& grid)
    {
        std::ofstream file(fileName);
        if (!file)
            GRIDLIB_THROW("Can not create file: " + fileName);
        writeJsonGrid(file, grid);
    }
}
