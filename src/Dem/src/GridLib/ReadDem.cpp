//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadDem.hpp"

#include <filesystem>
#include <fstream>
#include "DemReader.hpp"

namespace GridLib
{
    constexpr float METERS_PER_FOOT = 0.3048;
    constexpr int16_t UNKNOWN = -32767;

    Unit from_dem_unit(int16_t unit)
    {
        switch (unit)
        {
        case 1:
            return Unit::FEET;
        case 2:
            return Unit::METERS;
        case 3:
            return Unit::ARC_SECONDS;
        default:
            return Unit::UNDEFINED;
        }
    }

    Grid read_dem(std::istream& stream,
                  Unit vertical_unit,
                  const ProgressCallback& progress_callback)
    {
        Grid grid;
        DemReader reader(stream);
        auto& a = reader.record_a();
        auto h_unit = from_dem_unit(a.horizontal_unit.value_or(0));
        double c_res = a.x_resolution.value_or(1.0);
        double r_res = a.y_resolution.value_or(1.0);

        auto v_unit = from_dem_unit(a.vertical_unit.value_or(0));
        float v_res = a.z_resolution.value_or(1.0);

        if (a.longitude && a.latitude)
        {
            grid.set_spherical_coords(
                SphericalCoords{
                    to_degrees(*a.latitude),
                    to_degrees(*a.longitude)
                });
        }
        if (const auto& c = a.quadrangle_corners[0])
        {
            grid.set_planar_coords(
                PlanarCoords{
                    c->easting, c->northing, 0.0,
                    a.ref_sys_zone.value_or(0)
                });
        }
        if (a.horizontal_datum)
        {
            grid.set_reference_system(
                ReferenceSystem{
                    *a.horizontal_datum,
                    a.vertical_datum.value_or(0)
                });
        }
        double factor = 1.0;
        if (vertical_unit == Unit::METERS)
        {
            if (v_unit == Unit::FEET)
            {
                factor = v_res * METERS_PER_FOOT;
                v_res = 1.0;
                v_unit = Unit::METERS;
            }
            else
            {
                factor = v_res;
                v_res = 1.0;
            }

            if (h_unit == Unit::FEET)
            {
                r_res *= METERS_PER_FOOT;
                c_res *= METERS_PER_FOOT;
                h_unit = Unit::METERS;
            }
        }
        else if (vertical_unit == Unit::FEET)
        {
            if (v_unit == Unit::METERS)
            {
                factor = v_res / METERS_PER_FOOT;
                v_res = 1.0;
                v_unit = Unit::FEET;
            }
            else
            {
                factor = v_res;
                v_res = 1.0;
            }

            if (h_unit == Unit::METERS)
            {
                r_res *= 1.0 / METERS_PER_FOOT;
                c_res *= 1.0 / METERS_PER_FOOT;
                h_unit = Unit::FEET;
            }
        }

        auto rot = a.rotation_angle.value_or(0);
        auto row_axis = rotate(Xyz::Vector2D(0.0, r_res), rot);
        grid.set_row_axis({Xyz::make_vector3(row_axis, 0.0), h_unit});
        auto col_axis = rotate(Xyz::Vector2D(c_res, 0.0), rot);
        grid.set_column_axis({Xyz::make_vector3(col_axis, 0.0), h_unit});
        grid.set_vertical_axis({{0, 0, v_res}, v_unit});
        grid.set_unknown_elevation(UNKNOWN * factor);

        Chorasmia::MutableArrayView2D<float> values;
        auto rows = a.rows.value_or(1);
        auto cols = a.columns.value_or(1);
        while (auto b = reader.next_record_b())
        {
            if (values.empty())
            {
                if (rows == 1)
                    rows = b->rows;
                // DEM files uses columns as the primary dimension, while Grid
                // uses rows.
                grid.resize(cols, rows);
                values = grid.elevations();
            }
            for (int i = 0; i < b->columns; ++i)
            {
                for (int j = 0; j < b->rows; ++j)
                {
                    auto elev = b->elevations[i * b->rows + j];
                    values(i + b->column - 1, j + b->row - 1) = elev * factor;
                }
            }
            if (progress_callback && !progress_callback(b->column, cols))
                return {};
        }

        return grid;
    }

    Grid read_dem(const std::string& file_name,
                  Unit vertical_unit,
                  const ProgressCallback& progress_callback)
    {
        std::ifstream file(file_name, std::ios::binary);
        return read_dem(file, vertical_unit, progress_callback);
    }

    bool is_dem(const std::string& file_name)
    {
        auto ext = std::filesystem::path(file_name).extension().string();
        constexpr char SUFFIX[] = {'.', 'D', 'E', 'M'};
        return std::equal(ext.begin(), ext.end(),
                          std::begin(SUFFIX), std::end(SUFFIX),
                          [](auto a, auto b) { return toupper(a) == b; });
    }
}
