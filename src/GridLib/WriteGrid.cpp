//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-12-03.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/WriteGrid.hpp"
#include <Yson/JsonWriter.hpp>
#include <fstream>
#include <GridLib/GridLibException.hpp>

namespace GridLib
{
    void write_json(Yson::Writer& writer, const Coordinates& coords)
    {
        writer.beginObject();
        writer.key("model").beginObject()
            .key("x").value(coords.model[0])
            .key("y").value(coords.model[1])
            .key("z").value(coords.model[2])
            .endObject();
        writer.key("grid").beginObject()
            .key("row").value(coords.grid[0])
            .key("column").value(coords.grid[1])
            .endObject();
        if (coords.planar)
        {
            writer.key("planar").beginObject()
                .key("easting").value((*coords.planar)[0])
                .key("northing").value((*coords.planar)[1])
                .endObject();
        }
        if (coords.geographic)
        {
            writer.key("geographic").beginObject()
                .key("latitude").value((*coords.geographic)[0])
                .key("longitude").value((*coords.geographic)[1])
                .endObject();
        }
        writer.endObject();
    }

    void write_json(Yson::Writer& writer, const CoordinateReferenceSystem& ref_sys)
    {
        if (!ref_sys)
            return;

        writer.beginObject();
        if (ref_sys.projected)
            writer.key("projected").value(ref_sys.projected);
        if (ref_sys.vertical)
            writer.key("vertical").value(ref_sys.vertical);
        if (ref_sys.geographic)
            writer.key("geographic").value(ref_sys.geographic);
        if (ref_sys.zone)
            writer.key("zone").value(ref_sys.zone);
        writer.endObject();
    }

    void write_json(Yson::Writer& writer, const Xyz::Vector2D& vec)
    {
        using namespace Yson;
        writer.beginArray(JsonParameters(JsonFormatting::FLAT))
            .value(vec[0])
            .value(vec[1])
            .endArray();
    }

    void write_json(Yson::Writer& writer, const Xyz::Vector3D& vec)
    {
        using namespace Yson;
        writer.beginArray(JsonParameters(JsonFormatting::FLAT))
            .value(vec[0])
            .value(vec[1])
            .value(vec[2])
            .endArray();
    }

    void write_metadata(Yson::Writer& writer, const GridView& grid)
    {
        writer.beginObject();
        writer.key("row_count").value(uint64_t(grid.row_count()));
        writer.key("column_count").value(uint64_t(grid.col_count()));
        writer.key("row_axis");
        write_json(writer, grid.row_axis());
        writer.key("column_axis");
        write_json(writer, grid.col_axis());
        writer.key("vertical_axis");
        write_json(writer, grid.vertical_axis());

        writer.key("horizontal_unit").value(std::string(to_string(grid.horizontal_unit())));
        writer.key("vertical_unit").value(std::string(to_string(grid.vertical_unit())));

        writer.key("coordinates");
        write_json(writer, grid.coordinates());

        writer.key("reference_system");
        write_json(writer, grid.reference_system());

        writer.endObject();
    }

    void write_json(Yson::Writer& writer,
                    const Chorasmia::ArrayView2D<float>& values,
                    std::optional<float> null_value)
    {
        writer.beginArray();
        for (const auto& row : values)
        {
            writer.beginArray({
                Yson::JsonParameters(10),
                Yson::UBJsonParameters(ptrdiff_t(row.size()))
            });
            for (const auto& value : row)
            {
                if (!null_value || value != *null_value)
                    writer.value(value);
                else
                    writer.null();
            }
            writer.endArray();
        }
        writer.endArray();
    }

    void write_json(Yson::Writer& writer, const GridView& grid)
    {
        writer.beginObject();
        writer.key("metadata");
        write_metadata(writer, grid);
        writer.key("elevations");
        write_json(writer, grid.elevations(), grid.unknown_elevation());
        writer.endObject();
    }

    void write_json(std::ostream& stream, const GridView& grid)
    {
        Yson::JsonWriter writer(stream, Yson::JsonFormatting::FORMAT);
        writer.setNonFiniteFloatsEnabled(true);
        write_json(writer, grid);
    }

    void write_json(const std::string& file_name, const GridView& grid)
    {
        std::ofstream file(file_name);
        if (!file)
            GRIDLIB_THROW("Can not create file: " + file_name);
        write_json(file, grid);
    }
}
