//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-11-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/MultiGridReader.hpp"

#include <Chorasmia/ArrayView2DAlgorithms.hpp>

#include "GridLib/GridLibException.hpp"
#include "GridLib/PositionTransformer.hpp"
#include "GridLib/ReadGrid.hpp"
#include "Utilities/TemporaryFile.hpp"

namespace GridLib
{
    namespace
    {
        template <std::floating_point T>
        T get_fraction(T value)
        {
            T int_part;
            return std::modf(value, &int_part);
        }

        [[nodiscard]] SignedIndex
        get_insertion_point(const SpatialInfo& spatial_info,
                            const PositionTransformer& dst_trans)
        {
            const PositionTransformer src_trans(spatial_info.matrix,
                                                spatial_info.tie_point);
            const auto wp = src_trans.grid_to_world({0, 0});
            const auto gp = dst_trans.world_to_grid(wp);

            if (std::abs(get_fraction(gp.x())) > 1e-6
                || std::abs(get_fraction(gp.y())) > 1e-6)
            {
                GRIDLIB_THROW("Grid's position is not aligned to the target grid.");
            }

            return {
                static_cast<int64_t>(std::round(gp.x())),
                static_cast<int64_t>(std::round(gp.y()))
            };
        }
    }

    struct GridData
    {
        std::filesystem::path filename;
        std::streampos temp_file_offset = 0;
        SignedExtent extent;
        Xyz::Vector2D tie_point;
        SpatialInfo spatial_info;
        size_t z = 0;
    };

    struct MultiGridReader::Data
    {
        std::vector<GridData> grids;
        TemporaryFile temp_file;
        SignedExtent extent;
        std::vector<float> buffer;
    };

    MultiGridReader::MultiGridReader()
        : data_(std::make_unique<Data>())
    {
    }

    MultiGridReader::~MultiGridReader() = default;

    MultiGridReader::MultiGridReader(MultiGridReader&& other) noexcept
        : data_(std::move(other.data_))
    {
    }

    MultiGridReader& MultiGridReader::operator=(MultiGridReader&& other) noexcept
    {
        if (this == &other)
            return *this;

        data_ = std::move(other.data_);
        return *this;
    }

    Size MultiGridReader::size() const
    {
        return cast<size_t>(data_->extent.size);
    }

    void MultiGridReader::read_grid(const std::filesystem::path& filename)
    {
        assert_data();
        try
        {
            auto grid = GridLib::read_grid(filename, GridFileType::AUTO_DETECT);
            if (!grid.values().empty())
            {
                assert_compatible_grid(grid);

                auto& stream = data_->temp_file.stream();
                const auto temp_file_pos = stream.tellp();
                auto [rows, cols] = grid.size();
                stream.write(reinterpret_cast<const char*>(grid.values().data()),
                             std::streamsize(rows * cols * sizeof(float)));
                SignedIndex origin;
                if (!data_->grids.empty())
                {
                    const auto& first = data_->grids.front();
                    origin = get_insertion_point(
                        grid.spatial_info(),
                        PositionTransformer(
                            first.spatial_info.matrix,
                            first.tie_point));
                }
                data_->grids.push_back({
                    filename,
                    temp_file_pos,
                    {origin, cast<int64_t>(grid.size())},
                    grid.tie_point(),
                    grid.spatial_info(),
                    data_->grids.size()
                });

                const auto min = get_min(data_->extent.min_index(), origin);
                const auto max = get_max(data_->extent.max_index(),
                                         origin + cast<int64_t>(grid.size()));
                data_->extent = {min, max - min};
            }
        }
        catch (const std::exception&)
        {
            std::throw_with_nested(GRIDLIB_EXCEPTION("Error reading grid from file: "
                + filename.string()));
        }
    }

    bool MultiGridReader::has_data(const Extent& extent) const
    {
        assert_data();
        auto internal_extent = cast<int64_t>(extent);
        internal_extent.origin += data_->extent.origin;
        for (const auto& grid_data : data_->grids)
        {
            if (get_intersection(internal_extent, grid_data.extent))
                return true;
        }
        return false;
    }

    Grid MultiGridReader::get_grid(Extent extent) const
    {
        assert_data();
        extent = clamp(extent, cast<size_t>(data_->extent.size));
        auto internal_extent = cast<int64_t>(extent);
        internal_extent.origin += data_->extent.origin;

        Grid result(cast<size_t>(internal_extent.size));
        const auto& first = data_->grids.front();
        const auto& first_si = first.spatial_info;
        auto& si = result.spatial_info();
        si = first_si;
        si.tie_point = {0, 0};

        auto offsets = to_vector<double>(internal_extent.origin)
                       - (first_si.tie_point
                          + to_vector<double>(first.extent.origin));
        const auto location = first_si.location()
                              + offsets[0] * first_si.column_axis()
                              + offsets[1] * first_si.row_axis();
        si.set_location(location);

        for (const auto& grid_data : data_->grids)
            load_and_copy_grid_data(result, grid_data, internal_extent);

        return result;
    }

    void MultiGridReader::load_and_copy_grid_data(Grid& result,
                                                  const GridData& grid_data,
                                                  const SignedExtent& extent) const
    {
        auto overlap = get_intersection(extent, grid_data.extent);
        if (!overlap)
            return;

        const auto src_start = cast<size_t>(overlap->origin - grid_data.extent.origin);
        const auto dst_start = cast<size_t>(overlap->origin - extent.origin);
        const auto copy_size = cast<size_t>(overlap->size);

        const auto src_size = cast<size_t>(grid_data.extent.size);
        const auto buffer_size = get_array_size(src_size);
        data_->buffer.resize(buffer_size);
        data_->temp_file.stream().seekg(grid_data.temp_file_offset);
        auto stream_size = std::streamsize(buffer_size * sizeof(float));
        data_->temp_file.stream().read(reinterpret_cast<char*>(data_->buffer.data()), stream_size);

        const auto src = Chorasmia::ArrayView2D(data_->buffer.data(), src_size)
            .subarray({src_start, copy_size});
        const auto dst = result.values().subarray({dst_start, copy_size});
        Chorasmia::copy(src, dst, Chorasmia::Index2DMode::ROWS);
    }

    void MultiGridReader::assert_compatible_grid(const Grid& grid) const
    {
        if (data_->grids.empty())
            return;

        const auto& first = data_->grids.front();
        const auto& si = grid.spatial_info();
        if (si.row_axis() != first.spatial_info.row_axis())
            GRIDLIB_THROW("The row axes don't match.");
        if (si.column_axis() != first.spatial_info.column_axis())
            GRIDLIB_THROW("The column axes don't match.");
        if (si.vertical_axis() != first.spatial_info.vertical_axis())
            GRIDLIB_THROW("The vertical axes don't match.");
        if (si.horizontal_unit != first.spatial_info.horizontal_unit)
            GRIDLIB_THROW("The horizontal units don't match.");
        if (si.vertical_unit != first.spatial_info.vertical_unit)
            GRIDLIB_THROW("The vertical units don't match.");
        if (si.crs != first.spatial_info.crs)
            GRIDLIB_THROW("The coordinate reference systems don't match.");
    }

    void MultiGridReader::assert_data() const
    {
        if (!data_)
            GRIDLIB_THROW("MultiGridReader data is null.");
    }
}
