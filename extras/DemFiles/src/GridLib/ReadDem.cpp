//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-11-13.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadDem.hpp"
#include "DemReader.hpp"

namespace GridLib
{
    constexpr float METERS_PER_FOOT = 0.3048;
    constexpr int16_t UNKNOWN = -32767;

    Unit fromDemUnit(int16_t unit)
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

    Grid readDem(std::istream& stream,
                 Unit desired_unit,
                 const ProgressCallback& progress_callback)
    {
        Grid grid;
        DemReader reader(stream);
        auto& a = reader.record_a();
        auto hUnit = fromDemUnit(a.horizontal_unit.value_or(0));
        double cRes = a.x_resolution.value_or(1.0);
        double rRes = a.y_resolution.value_or(1.0);

        auto vUnit = fromDemUnit(a.vertical_unit.value_or(0));
        double vRes = a.z_resolution.value_or(1.0);

        if (a.longitude && a.latitude)
        {
            grid.setSphericalCoords(
                SphericalCoords{to_degrees(*a.latitude),
                                         to_degrees(*a.longitude)});
        }
        if (const auto& c = a.quadrangle_corners[0])
        {
            grid.setPlanarCoords(
                PlanarCoords{c->easting, c->northing,
                                      a.ref_sys_zone.value_or(0)});
        }
        if (a.horizontal_datum)
        {
            grid.setReferenceSystem(
                ReferenceSystem{*a.horizontal_datum,
                                         a.vertical_datum.value_or(0)});
        }
        double factor = 1.0;
        if (desired_unit == Unit::METERS)
        {
            if (vUnit == Unit::FEET)
            {
                factor = vRes * METERS_PER_FOOT;
                vRes = 1.0;
                vUnit = Unit::METERS;
            }
            else
            {
                factor = vRes;
                vRes = 1.0;
            }

            if (hUnit == Unit::FEET)
            {
                rRes *= METERS_PER_FOOT;
                cRes *= METERS_PER_FOOT;
                hUnit = Unit::METERS;
            }
        }
        else if (desired_unit == Unit::FEET)
        {
            if (vUnit == Unit::METERS)
            {
                factor = vRes / METERS_PER_FOOT;
                vRes = 1.0;
                vUnit = Unit::FEET;
            }
            else
            {
                factor = vRes;
                vRes = 1.0;
            }

            if (hUnit == Unit::METERS)
            {
                rRes *= 1.0 / METERS_PER_FOOT;
                cRes *= 1.0 / METERS_PER_FOOT;
                hUnit = Unit::FEET;
            }
        }

        auto rot = a.rotation_angle.value_or(0);
        auto rowAxis = rotate(Xyz::makeVector2(0.0, rRes), rot);
        grid.setRowAxis({Xyz::makeVector3(rowAxis, 0.0), hUnit});
        auto colAxis = rotate(Xyz::makeVector2(cRes, 0.0), rot);
        grid.setColumnAxis({Xyz::makeVector3(colAxis, 0.0), hUnit});
        grid.setVerticalAxis({{0, 0, vRes}, vUnit});
        grid.setUnknownElevation(UNKNOWN * factor);

        Chorasmia::MutableArrayView2D<double> values;
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
}
