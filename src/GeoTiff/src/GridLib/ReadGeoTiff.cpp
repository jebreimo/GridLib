//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-01-05.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/ReadGeoTiff.hpp"

#include <Yimage/ReadImage.hpp>
#include <Yimage/Tiff/GeoTiffMetadata.hpp>
#include <Yimage/Tiff/ReadTiff.hpp>
#include "GridLib/Utilities/CoordinateSystem.hpp"

namespace GridLib
{
    namespace
    {
        Unit get_horizontal_unit(const Yimage::GeoTiffMetadata& metadata)
        {
            if (metadata.projected_linear_units)
                return epsg_unit_to_unit(metadata.projected_linear_units);

            int epsg = metadata.projected_crs
                           ? metadata.projected_crs
                           : metadata.geodetic_crs;
            return epsg_crs_to_horizontal_unit(epsg);
        }

        Unit get_vertical_unit(const Yimage::GeoTiffMetadata& metadata)
        {
            if (metadata.vertical_units)
                return epsg_unit_to_unit(metadata.vertical_units);

            int epsg = metadata.vertical_crs
                           ? metadata.vertical_crs
                           : metadata.projected_crs
                                 ? metadata.projected_crs
                                 : metadata.geodetic_crs;
            return epsg_crs_to_vertical_unit(epsg);
        }

        Grid create_grid(const Yimage::Image& img)
        {
            auto metadata = dynamic_cast<const Yimage::GeoTiffMetadata*>(img.metadata());
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

            result.set_reference_system(CoordinateReferenceSystem{
                metadata->projected_crs,
                metadata->vertical_crs,
                metadata->geodetic_crs,
                0
            });

            Coordinates coordinates;
            coordinates.model = {
                metadata->model_tie_point[3],
                metadata->model_tie_point[4],
                metadata->model_tie_point[5]
            };
            coordinates.grid = {
                metadata->model_tie_point[1],
                metadata->model_tie_point[0]
            };

            if (metadata->projected_crs)
            {
                coordinates.planar = Xyz::Vector2D(coordinates.model[0],
                                                   coordinates.model[1]);
            }

            if (metadata->geodetic_crs)
            {
                coordinates.geographic = Xyz::Vector2D(coordinates.model[1],
                                                       coordinates.model[0]);
            }
            else if (metadata->projected_crs)
            {
                coordinates.geographic = get_geographic_coords(coordinates.model[0],
                                                               coordinates.model[1],
                                                               metadata->projected_crs);
            }

            auto xs = metadata->model_pixel_scale[0];
            if (xs == 0)
                xs = 1.0;
            result.set_row_axis({xs, 0.0, 0.0});
            auto ys = metadata->model_pixel_scale[1];
            if (ys == 0)
                ys = 1.0;
            result.set_column_axis({0.0, -ys, 0.0});
            auto zs = metadata->model_pixel_scale[2];
            if (zs == 0)
                zs = 1.0;
            result.set_vertical_axis({0.0, 0.0, zs});

            result.set_horizontal_unit(get_horizontal_unit(*metadata));
            result.set_vertical_unit(get_vertical_unit(*metadata));

            result.set_coordinates(coordinates);

            return result;
        }
    }

    Grid read_geotiff(std::istream& stream)
    {
        auto img = Yimage::read_tiff(stream);
        return create_grid(img);
    }

    Grid read_geotiff(const std::filesystem::path& path)
    {
        auto img = Yimage::read_tiff(path);
        return create_grid(img);
    }

    bool is_tiff(const std::filesystem::path& path)
    {
        return Yimage::get_image_format(path) == Yimage::ImageFormat::TIFF;
    }
}
