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
        SignedIndex origin;
        Size size;
        Xyz::Vector2D tie_point;
        SpatialInfo spatial_info;
        size_t z = 0;
    };

    struct MultiGridReader::Data
    {
        std::vector<GridData> grids;
        TemporaryFile temp_file;
        SignedIndex min_index;
        SignedIndex max_index;
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
        return cast<size_t>(data_->max_index - data_->min_index);
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
                    origin,
                    grid.size(),
                    grid.tie_point(),
                    grid.spatial_info(),
                    data_->grids.size()
                });

                data_->min_index = get_min(data_->min_index, origin);
                data_->max_index = get_max(data_->max_index,
                                           origin + cast<int64_t>(grid.size()));
            }
        }
        catch (const std::exception& e)
        {
            std::throw_with_nested(GRIDLIB_EXCEPTION("Error reading grid from file: "
                + filename.string()));
        }
    }

    bool overlaps(SignedIndex min_index,
                  SignedIndex max_index,
                  const GridData& grid_data)
    {
        const auto grid_min_index = grid_data.origin;
        const auto grid_max_index = grid_min_index
                                    + cast<int64_t>(grid_data.size);
        return (grid_min_index.row < max_index.row
                && grid_max_index.row > min_index.row
                && grid_min_index.column < max_index.column
                && grid_max_index.column > min_index.column);
    }

    bool MultiGridReader::has_data(Index index, Size size) const
    {
        assert_data();
        const auto min_index = cast<int64_t>(index) + data_->min_index;
        const auto max_index = min_index + cast<int64_t>(size);
        for (const auto& grid_data : data_->grids)
        {
            if (overlaps(min_index, max_index, grid_data))
                return true;
        }
        return false;
    }

    Grid MultiGridReader::get_grid(Index index, Size size) const
    {
        assert_data();
        const auto min_index = cast<int64_t>(index) + data_->min_index;
        const auto grid_size = data_->max_index - min_index;
        const auto result_size = cast<
            int64_t>(get_min(size, cast<size_t>(grid_size)));
        const auto max_index = min_index + result_size;

        Grid result(cast<size_t>(max_index - min_index));

        for (const auto& grid_data : data_->grids)
            load_and_copy_grid_data(result, grid_data, min_index, max_index);

        return result;
    }

    void MultiGridReader::load_and_copy_grid_data(Grid& result,
                                                  const GridData& grid_data,
                                                  const SignedIndex& min_index,
                                                  const SignedIndex& max_index) const
    {
        if (!overlaps(min_index, max_index, grid_data))
            return;

        const auto min = get_max(min_index, grid_data.origin);
        const auto max = get_min(max_index,
                                 grid_data.origin + cast<int64_t>(grid_data.size));
        const auto src_start = cast<size_t>(min - grid_data.origin);
        const auto dst_start = cast<size_t>(min - min_index);
        const auto copy_size = cast<size_t>(max - min);
        data_->buffer.resize(grid_data.size.rows * grid_data.size.columns);
        data_->temp_file.stream().seekg(grid_data.temp_file_offset);
        auto buffer_size = std::streamsize(
            grid_data.size.row * grid_data.size.column * sizeof(float));
        data_->temp_file.stream().read(reinterpret_cast<char*>(data_->buffer.data()), buffer_size);

        auto src = Chorasmia::ArrayView2D(data_->buffer.data(), grid_data.size)
            .subarray({src_start, copy_size});
        auto dst = result.values().subarray({dst_start, copy_size});
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
