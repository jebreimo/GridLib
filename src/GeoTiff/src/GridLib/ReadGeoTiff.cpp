//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadGeoTiff.hpp"

#include <Yimage/Tiff/GeoTiffMetadata.hpp>
#include <Yimage/Tiff/ReadTiff.hpp>
#include "GridLib/Utilities/CoordinateSystem.hpp"

namespace GridLib
{
    Grid read_geotiff(std::istream& stream)
    {
        auto img = Yimage::read_tiff(stream);

        return {};
    }

    Grid read_geotiff(const std::string& file_name)
    {
        auto img = Yimage::read_tiff(file_name);
        auto metadata = dynamic_cast<Yimage::GeoTiffMetadata*>(img.metadata());
        if (!metadata || img.pixel_type() != Yimage::PixelType::MONO_FLOAT_32)
            return {};

        std::vector<float> elevations(img.width());

        Grid result(img.height(), img.width());
        for (size_t i = 0; i < img.height(); ++i)
        {
            auto [src, src_end] = img.row(i);
            std::copy(src, src_end, reinterpret_cast<unsigned char*>(elevations.data()));
            const auto dst = result.elevations()[i];
            std::copy(elevations.begin(), elevations.end(), dst.begin());
        }

        auto epsg = metadata->projected_crs;
        if (epsg)
        {
            result.set_reference_system(ReferenceSystem{epsg, 0});
            auto x = metadata->model_tie_point[3];
            auto y = metadata->model_tie_point[4];
            if (x != 0 || y != 0)
            {
                result.set_planar_coords(get_planar_coords(x, y, epsg));
                result.set_spherical_coords(get_spherical_coords(x, y, epsg));
            }
        }

        if (auto xs = metadata->model_pixel_scale[0]; xs != 0)
            result.set_row_axis({{xs, 0.0, 0.0}, Unit::METERS});
        if (auto ys = metadata->model_pixel_scale[1]; ys != 0)
            result.set_column_axis({{0.0, -ys, 0.0}, Unit::METERS});
        if (auto zs = metadata->model_pixel_scale[2]; zs != 0)
            result.set_vertical_axis({{0.0, 0.0, zs}, Unit::METERS});

        return result;
    }

    bool is_geotiff(const std::string& file_name)
    {
        auto metadata = Yimage::read_tiff_metadata(file_name);
        return dynamic_cast<Yimage::GeoTiffMetadata*>(metadata.get()) != nullptr;
    }
}
