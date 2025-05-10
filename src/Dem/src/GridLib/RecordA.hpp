//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-09-27.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>

namespace GridLib
{
    struct DegMinSec
    {
        int16_t degree = 0;
        int16_t minute = 0;
        float second = 0;
    };

    double to_degrees(const DegMinSec& dms);

    struct CartesianCoordinates
    {
        double easting = 0;
        double northing = 0;
    };

    struct RecordA
    {
        std::string file_name;
        std::string text;
        std::optional<DegMinSec> longitude;
        std::optional<DegMinSec> latitude;
        std::optional<char> process_code;
        std::string sectional_indicator;
        std::string origin_code;
        std::optional<int16_t> dem_level_code;
        std::optional<int16_t> elevation_pattern_code;
        std::optional<int16_t> ref_sys;
        std::optional<int16_t> ref_sys_zone;
        std::optional<double> map_projection_params[15];
        std::optional<int16_t> horizontal_unit;
        std::optional<int16_t> vertical_unit;
        std::optional<int16_t> polygon_sides;
        std::optional<CartesianCoordinates> quadrangle_corners[4];
        std::optional<double> min_elevation;
        std::optional<double> max_elevation;
        std::optional<double> rotation_angle;
        std::optional<int16_t> elevation_accuracy;
        std::optional<double> x_resolution;
        std::optional<double> y_resolution;
        std::optional<double> z_resolution;
        std::optional<int16_t> rows;
        std::optional<int16_t> columns;
        std::optional<int16_t> largest_contour_interval;
        std::optional<int8_t> largest_contour_interval_units;
        std::optional<int16_t> smallest_contour_interval;
        std::optional<int8_t> smallest_contour_interval_units;
        std::optional<int16_t> data_source_year;
        std::optional<int16_t> data_completion_year;
        std::optional<char> inspection_flag;
        std::optional<int8_t> data_validation_flag;
        std::optional<int8_t> suspect_and_void_area_flag;
        std::optional<int8_t> vertical_datum;
        std::optional<int8_t> horizontal_datum;
        std::optional<int16_t> data_edition;
        std::optional<int16_t> percent_void;
        std::optional<int32_t> edge_match_flag;
        std::optional<double> vertical_datum_shift;
    };

    void print(const RecordA& rec, std::ostream& os);

    class FortranReader;

    [[nodiscard]]
    RecordA read_record_a(FortranReader& reader);
}
