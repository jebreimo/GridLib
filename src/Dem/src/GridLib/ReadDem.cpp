//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-13.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadDem.hpp"

#include <filesystem>
#include <fstream>
#include "DemReader.hpp"
#include "GridLib/GridLibException.hpp"
#include "GridLib/Utilities/ReadOnlyStreamBuffer.hpp"

namespace GridLib
{
    constexpr float METERS_PER_FOOT = 0.3048;
    constexpr int16_t UNKNOWN = -32767;

    float to_float(double value)
    {
        if (value > std::numeric_limits<float>::max() ||
            value < -std::numeric_limits<float>::max())
        {
            GRIDLIB_THROW("Value out of range: " + std::to_string(value));
        }
        return static_cast<float>(value);
    }

    Unit from_dem_unit(int16_t unit)
    {
        switch (unit)
        {
        case 1:
            return Unit::FOOT;
        case 2:
            return Unit::METER;
        case 3:
            return Unit::ARC_SECOND;
        default:
            return Unit::UNDEFINED;
        }
    }

    int get_vertical_epsg(const RecordA& a)
    {
        if (const auto vd = a.vertical_datum.value_or(0); vd != 1)
            GRIDLIB_THROW("Unsupported vertical datum: " + std::to_string(vd));

        return 0;
    }

    Crs get_crs(const RecordA& a)
    {
        if (const int hd = a.horizontal_datum.value_or(0); hd != 3)
            GRIDLIB_THROW("Unsupported horizontal datum: " + std::to_string(hd));

        const int v = get_vertical_epsg(a);
        const auto ref_sys = a.ref_sys.value_or(0);
        if (ref_sys == 0)
            return {4326, v, CrsType::GEOGRAPHIC, CrsLibrary::EPSG};

        if (ref_sys == 1)
        {
            int zone = a.ref_sys_zone.value_or(0);
            // Assuming Northern Hemisphere
            return {32600 + zone, v, CrsType::PROJECTED, CrsLibrary::EPSG};
        }

        GRIDLIB_THROW("Unsupported reference system: " + std::to_string(ref_sys));
    }

    Grid read_dem(std::istream& stream)
    {
        Grid grid;
        DemReader reader(stream);
        auto& a = reader.record_a();

        auto rows = a.rows.value_or(1);
        auto cols = a.columns.value_or(1);

        auto crs = get_crs(a);
        auto z = a.vertical_datum_shift.value_or(0);

        auto& model = grid.spatial_info();

        std::vector<SpatialTiePoint> spatial_ties;
        if (a.longitude && a.latitude)
        {
            spatial_ties.push_back({
                {0, 0},
                {to_degrees(*a.latitude), to_degrees(*a.longitude), z},
                {4326, 0, CrsType::GEOGRAPHIC, CrsLibrary::EPSG}
            });
        }
        if (const auto& c = a.quadrangle_corners[0])
        {
            model.set_location({c->easting, c->northing, z});
            spatial_ties.push_back({
                {0, 0},
                {c->easting, c->northing, z},
                crs
            });
        }
        if (const auto& c = a.quadrangle_corners[1])
        {
            spatial_ties.push_back({
                {double(rows - 1), 0},
                {c->easting, c->northing, z},
                crs
            });
        }
        if (const auto& c = a.quadrangle_corners[2])
        {
            spatial_ties.push_back({
                {double(rows - 1), double(cols - 1)},
                {c->easting, c->northing, z},
                crs
            });
        }
        if (const auto& c = a.quadrangle_corners[3])
        {
            spatial_ties.push_back({
                {0, double(cols - 1)},
                {c->easting, c->northing, z},
                crs
            });
        }

        model.extra_tie_points = std::move(spatial_ties);

        auto h_unit = from_dem_unit(a.horizontal_unit.value_or(0));
        double c_res = a.x_resolution.value_or(1.0);
        double r_res = a.y_resolution.value_or(1.0);

        auto v_unit = from_dem_unit(a.vertical_unit.value_or(0));
        float v_res = to_float(a.z_resolution.value_or(1.0));

        auto rot = a.rotation_angle.value_or(0);
        auto row_axis = rotate(Xyz::Vector2D(0.0, r_res), rot);
        model.set_row_axis(Xyz::make_vector3(row_axis, 0.0));
        auto col_axis = rotate(Xyz::Vector2D(c_res, 0.0), rot);
        model.set_column_axis(Xyz::make_vector3(col_axis, 0.0));
        model.set_vertical_axis({0, 0, v_res});
        model.crs = crs;

        model.horizontal_unit = h_unit;
        model.vertical_unit = v_unit;

        Chorasmia::MutableArrayView2D<float> values;
        while (auto b = reader.next_record_b())
        {
            if (values.empty())
            {
                if (rows == 1)
                    rows = b->rows;
                // DEM files uses columns as the primary dimension, while Grid
                // uses rows.
                grid.resize({int64_t(cols), int64_t(rows)});
                values = grid.values();
            }
            for (int i = 0; i < b->columns; ++i)
            {
                for (int j = 0; j < b->rows; ++j)
                {
                    auto elev = b->elevations[i * b->rows + j];
                    float value = elev == UNKNOWN
                                      ? UNKNOWN_ELEVATION
                                      : float(elev) * v_res;
                    values(i + b->column - 1, j + b->row - 1) = value;
                }
            }
        }

        return grid;
    }

    Grid read_dem(const void* buffer, size_t size)
    {
        ReadOnlyStreamBuffer stream_buffer(static_cast<const char*>(buffer), size);
        std::istream stream(&stream_buffer);
        return read_dem(stream);
    }

    Grid read_dem(const std::filesystem::path& filename)
    {
        std::ifstream file(filename, std::ios::binary);
        return read_dem(file);
    }

    bool is_dem(const std::filesystem::path& filename)
    {
        auto ext = filename.extension().string();
        constexpr char SUFFIX[] = {'.', 'D', 'E', 'M'};
        return std::equal(ext.begin(), ext.end(),
                          std::begin(SUFFIX), std::end(SUFFIX),
                          [](auto a, auto b) { return toupper(a) == b; });
    }
}
