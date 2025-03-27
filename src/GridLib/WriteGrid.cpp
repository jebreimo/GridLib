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
    void write_json(Yson::Writer& writer, const PlanarCoords& coords)
    {
        writer.beginObject()
            .key("northing").value(coords.northing)
            .key("easting").value(coords.easting)
            .key("elevation").value(coords.elevation)
            .key("zone").value(coords.zone)
            .endObject();
    }

    void write_json(Yson::Writer& writer, const SphericalCoords& coords)
    {
        writer.beginObject()
            .key("latitude").value(coords.latitude)
            .key("longitude").value(coords.longitude)
            .endObject();
    }

    void write_json(Yson::Writer& writer, const ReferenceSystem& refSys)
    {
        writer.beginObject()
            .key("projected").value(refSys.projected);
        if (refSys.vertical)
            writer.key("vertical").value(refSys.vertical);
        writer.endObject();
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

    void write_json(Yson::Writer& writer, const Axis& axis)
    {
        using namespace Yson;
        writer.beginObject();
        writer.key("unit").value(std::string(to_string(axis.unit)));
        writer.key("direction");
        write_json(writer, axis.direction);
        writer.endObject();
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

        if (const auto& coords = grid.planar_coords())
        {
            writer.key("planar_coords");
            write_json(writer, *coords);
        }

        if (const auto& coords = grid.spherical_coords())
        {
            writer.key("spherical_coords");
            write_json(writer, *coords);
        }

        if (const auto& refSys = grid.reference_system())
        {
            writer.key("reference_system");
            write_json(writer, *refSys);
        }

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
