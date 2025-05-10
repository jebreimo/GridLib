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
#include "GridLib/Utilities/ReadOnlyStreamBuffer.hpp"

namespace GridLib
{
    namespace
    {
        Unit get_horizontal_unit(const Yimage::GeoTiffMetadata& metadata)
        {
            if (metadata.projected_linear_units)
                return epsg_unit_to_unit(metadata.projected_linear_units);

            const int epsg = metadata.projected_crs
                                 ? metadata.projected_crs
                                 : metadata.geodetic_crs;
            return epsg_crs_to_horizontal_unit(epsg);
        }

        Unit get_vertical_unit(const Yimage::GeoTiffMetadata& metadata)
        {
            if (metadata.vertical_units)
                return epsg_unit_to_unit(metadata.vertical_units);

            const int epsg = metadata.vertical_crs
                                 ? metadata.vertical_crs
                                 : metadata.projected_crs
                                 ? metadata.projected_crs
                                 : metadata.geodetic_crs;
            return epsg_crs_to_vertical_unit(epsg);
        }

        Xyz::Vector2D get_tie_point(const Yimage::GeoTiffMetadata& metadata)
        {
            return {
                metadata.model_tie_point[1],
                metadata.model_tie_point[0]
            };
        }

        Xyz::Vector3D get_location(const Yimage::GeoTiffMetadata& metadata)
        {
            return {
                metadata.model_tie_point[3],
                metadata.model_tie_point[4],
                metadata.model_tie_point[5]
            };
        }

        Crs get_crs(const Yimage::GeoTiffMetadata& metadata)
        {
            Crs result;

            if (metadata.projected_crs != 0)
            {
                result.type = CrsType::PROJECTED;
                result.code = metadata.projected_crs;
                result.library = CrsLibrary::EPSG;
                result.citation = metadata.projected_citation;
            }
            else if (metadata.geodetic_crs != 0)
            {
                result.type = CrsType::GEOGRAPHIC;
                result.code = metadata.geodetic_crs;
                result.library = CrsLibrary::EPSG;
                result.citation = metadata.geog_citation;
            }

            result.vertical_code = metadata.vertical_crs;

            return result;
        }

        Grid create_grid(const Yimage::Image& img)
        {
            const auto metadata = dynamic_cast<const Yimage::GeoTiffMetadata*>(img.metadata());
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

            const auto tie_point = get_tie_point(*metadata);
            result.set_model_tie_point(get_tie_point(*metadata));

            auto& model = result.model();

            const auto location = get_location(*metadata);
            model.set_location(location);

            std::vector<SpatialTiePoint> spatial_ties;

            const auto crs = get_crs(*metadata);
            model.crs = crs;
            spatial_ties.push_back({tie_point, location, crs});

            result.set_spatial_tie_points(std::move(spatial_ties));

            auto xs = metadata->model_pixel_scale[0];
            if (xs == 0)
                xs = 1.0;

            model.set_row_axis({xs, 0.0, 0.0});
            auto ys = metadata->model_pixel_scale[1];
            if (ys == 0)
                ys = 1.0;
            model.set_column_axis({0.0, -ys, 0.0});
            auto zs = metadata->model_pixel_scale[2];
            if (zs == 0)
                zs = 1.0;
            model.set_vertical_axis({0.0, 0.0, zs});

            model.horizontal_unit = get_horizontal_unit(*metadata);
            model.vertical_unit = get_vertical_unit(*metadata);

            if (!metadata->citation.empty())
                model.information.emplace_back("citation", metadata->citation);
            if (!metadata->gdal_metadata.empty())
                model.information.emplace_back("gdal", metadata->gdal_metadata);
            if (!metadata->date_time.empty())
                model.information.emplace_back("date", metadata->date_time);
            if (!metadata->artist.empty())
                model.information.emplace_back("artist", metadata->artist);
            if (!metadata->host_computer.empty())
                model.information.emplace_back("host_computer", metadata->host_computer);
            if (!metadata->software.empty())
                model.information.emplace_back("software", metadata->software);

            return result;
        }
    }

    Grid read_geotiff(std::istream& stream)
    {
        const auto img = Yimage::read_tiff(stream);
        return create_grid(img);
    }

    Grid read_geotiff(const void* buffer, size_t size)
    {
        ReadOnlyStreamBuffer stream_buffer(static_cast<const char*>(buffer), size);
        std::istream stream(&stream_buffer);
        return read_geotiff(stream);
    }

    Grid read_geotiff(const std::filesystem::path& path)
    {
        const auto img = Yimage::read_tiff(path);
        return create_grid(img);
    }

    bool is_tiff(const std::filesystem::path& path)
    {
        return Yimage::get_image_format(path) == Yimage::ImageFormat::TIFF;
    }
}
