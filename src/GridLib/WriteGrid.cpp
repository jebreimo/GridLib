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
    namespace
    {
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
    }

    void write_json(Yson::Writer& writer, const Crs& crs)
    {
        writer.beginObject();
        writer.key("code").value(crs.code);
        writer.key("vertical_code").value(crs.vertical_code);
        writer.key("type").value(to_string(crs.type));
        writer.key("library").value(to_string(crs.library));
        if (!crs.citation.empty())
            writer.key("citation").value(crs.citation);
        writer.endObject();
    }

    void write_json(Yson::Writer& writer,
                    const std::vector<std::pair<std::string, std::string>>& pairs)
    {
        if (pairs.empty())
            return;

        writer.beginObject();
        for (const auto& [key, value] : pairs)
        {
            writer.key(key).value(value);
        }
        writer.endObject();
    }

    void write_json(Yson::Writer& writer, const GridModel& model)
    {
        writer.beginObject();
        writer.key("location");
        write_json(writer, model.location());

        writer.key("column_axis");
        write_json(writer, model.column_axis());
        writer.key("row_axis");
        write_json(writer, model.row_axis());
        writer.key("vertical_axis");
        write_json(writer, model.vertical_axis());

        writer.key("horizontal_unit").value(to_string(model.horizontal_unit));
        writer.key("vertical_unit").value(to_string(model.vertical_unit));
        if (model.unknown_elevation)
            writer.key("unknown_elevation").value(*model.unknown_elevation);

        writer.key("crs");
        write_json(writer, model.crs);

        writer.key("information");
        write_json(writer, model.information);

        writer.endObject();
    }

    void write_json(Yson::Writer& writer, const std::vector<SpatialTiePoint>& points)
    {
        if (points.empty())
            return;

        writer.beginObject();
        for (const auto& point : points)
        {
            writer.beginObject();
            writer.key("grid_point");
            write_json(writer, point.grid_point);
            writer.key("location");
            write_json(writer, point.location);
            writer.key("crs");
            write_json(writer, point.crs);
            writer.endObject();
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

    void write_json(Yson::Writer& writer, const IGrid& grid)
    {
        writer.beginObject();
        writer.key("row_count").value(uint64_t(grid.row_count()));
        writer.key("column_count").value(uint64_t(grid.col_count()));
        writer.key("model_tie_point");
        write_json(writer, grid.model_tie_point());
        writer.key("model");
        write_json(writer, grid.model());
        writer.key("spatial_tie_points");
        write_json(writer, grid.spatial_tie_points());
        writer.key("elevations");
        write_json(writer, grid.elevations(), grid.model().unknown_elevation);
        writer.endObject();
    }

    void write_json(std::ostream& stream, const IGrid& grid)
    {
        Yson::JsonWriter writer(stream, Yson::JsonFormatting::FORMAT);
        writer.setNonFiniteFloatsEnabled(true);
        write_json(writer, grid);
    }

    void write_json(const std::string& file_name, const IGrid& grid)
    {
        std::ofstream file(file_name);
        if (!file)
            GRIDLIB_THROW("Can not create file: " + file_name);
        write_json(file, grid);
    }
}
