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
            using namespace Yson;
            writer.beginObject();
            writer.key("unit").value(std::string(to_string(axis.unit)));
            writer.key("direction")
                .beginArray(JsonParameters(JsonFormatting::FLAT))
                .value(axis.direction[0])
                .value(axis.direction[1])
                .value(axis.direction[2])
                .endArray();
            writer.endObject();
        }

        void writeMetadata(Yson::Writer& writer, const GridView& grid)
        {
            writer.beginObject();
            writer.key("row_count").value(uint64_t(grid.row_count()));
            writer.key("column_count").value(uint64_t(grid.col_count()));
            writer.key("row_axis");
            writeAxis(writer, grid.row_axis());
            writer.key("column_axis");
            writeAxis(writer, grid.col_axis());
            writer.key("vertical_axis");
            writeAxis(writer, grid.vertical_axis());

            if (const auto& coords = grid.planar_coords())
            {
                writer.key("planar_coords")
                    .beginObject()
                    .key("northing").value(coords->northing)
                    .key("easting").value(coords->easting)
                    .key("zone").value(coords->zone)
                    .endObject();
            }

            if (const auto& coords = grid.spherical_coords())
            {
                writer.key("spherical_coords")
                    .beginObject()
                    .key("latitude").value(coords->latitude)
                    .key("longitude").value(coords->longitude)
                    .endObject();
            }

            if (const auto& refSys = grid.reference_system())
            {
                writer.key("reference_system")
                    .beginObject()
                    .key("projected").value(refSys->projected);
                if (refSys->vertical)
                    writer.key("vertical").value(refSys->vertical);
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
                writer.beginArray({Yson::JsonParameters(10),
                                   Yson::UBJsonParameters(ptrdiff_t (row.size()))});
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

    void write_json_grid(std::ostream& stream, const GridView& grid)
    {
        Yson::JsonWriter writer(stream, Yson::JsonFormatting::FORMAT);
        writer.beginObject();
        writer.key("metadata");
        writeMetadata(writer, grid);
        writer.key("elevations");
        writeElevations(writer, grid.elevations(), grid.unknown_elevation());
        writer.endObject();
    }

    void write_json_grid(const std::string& file_name, const GridView& grid)
    {
        std::ofstream file(file_name);
        if (!file)
            GRIDLIB_THROW("Can not create file: " + file_name);
        write_json_grid(file, grid);
    }
}
