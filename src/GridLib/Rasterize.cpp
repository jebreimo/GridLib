//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-03-07.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Rasterize.hpp"
#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    namespace
    {
        using Color = Xyz::Vector<uint32_t, 4>;

        [[nodiscard]]
        uint32_t to_rgba(Color c)
        {
            return (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
        }

        [[nodiscard]]
        Color from_rgba(uint32_t rgba)
        {
            return Color{(rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF,
                         (rgba >> 8) & 0xFF, rgba & 0xFF};
        }

        void add_range(Chorasmia::IntervalMap<float, uint32_t>& map,
                       float from_height, float to_height,
                       const Color& from_color, const Color& to_color,
                       int32_t steps)
        {
            const auto delta = to_color - from_color;
            for (int32_t i = 0; i < int32_t(steps); ++i)
            {
                const auto c = from_color + (delta * i) / int32_t(steps - 1);
                map.insert(from_height + (to_height - from_height) * float(i) / float(steps),
                           from_height + (to_height - from_height) * float(i + 1) / float(steps),
                           to_rgba(c));
            }
        }
    }

    Chorasmia::IntervalMap<float, uint32_t> make_map_gradient(
        float sea_level_min,
        float sea_level_max,
        float ground_level_1_max,
        float ground_level_2_max,
        float ground_level_3_max,
        float ground_level_4_max,
        float ground_level_5_max)
    {
        Chorasmia::IntervalMap<float, uint32_t> map(0xFFFF8E5Au);
        map.insert(sea_level_max, 0xFFFCF2E9u);
        add_range(map, sea_level_min, sea_level_max, from_rgba(0xFFFF8E5Au),
                  from_rgba(0xFFFAEAD1u), 10);
        add_range(map, sea_level_max, ground_level_1_max, from_rgba(0xFF74907Cu),
                  from_rgba(0xFFD0E8D7u), 10);
        add_range(map, ground_level_1_max, ground_level_2_max, from_rgba(0xFFBDE8E5u),
                  from_rgba(0xFF6F888Cu), 10);
        add_range(map, ground_level_2_max, ground_level_3_max, from_rgba(0xFF69848Cu),
                  from_rgba(0xFF95B9D1u), 10);
        add_range(map, ground_level_3_max, ground_level_4_max, from_rgba(0xFF95A9C1u),
                  from_rgba(0xFFE3DCDCu), 10);
        add_range(map, ground_level_4_max, ground_level_5_max, from_rgba(0xFFECC8A6u),
                  from_rgba(0xFFFCF2E9u), 10);

        // Extend the sea a little bit:
        map.insert(sea_level_max, sea_level_max + 0.25f, 0xFFFAEAD1u);

        return map;
    }

    Chorasmia::IntervalMap<float, uint32_t> make_default_gradient_2500()
    {
        return make_map_gradient(2500, 0, 500, 1000, 1500, 2000, 2500);
    }

    Chorasmia::IntervalMap<float, uint32_t> make_default_gradient_9000()
    {
        return make_map_gradient(2500, 0, 1000, 2000, 3000, 5000, 9000);
    }

    namespace
    {
        enum class CardinalDirection
        {
            NORTH,
            EAST,
            SOUTH,
            WEST
        };

        CardinalDirection rotate(CardinalDirection dir, int times)
        {
            return CardinalDirection((int(dir) + times) % 4);
        }

        int get_rotation(CardinalDirection srcDir, CardinalDirection dstDir)
        {
            return (int(dstDir) + 4 - int(srcDir)) % 4;
        }

        bool is_flipped(CardinalDirection rowDir, CardinalDirection colDir)
        {
            return (int(rowDir) + 4 - int(colDir)) % 4 == 1;
        }

        bool are_orthogonal(CardinalDirection a, CardinalDirection b)
        {
            return std::abs(int(a) - int(b)) % 2 == 1;
        }

        Chorasmia::Index2DMode
        get_index_2d_mode(CardinalDirection src_row_dir,
                          CardinalDirection src_col_dir,
                          CardinalDirection dst_row_dir,
                          CardinalDirection dst_col_dir)
        {
            if (!are_orthogonal(src_row_dir, src_col_dir))
                GRIDLIB_THROW("Source coordinate axes are non-orthogonal.");
            if (!are_orthogonal(dst_row_dir, dst_col_dir))
                GRIDLIB_THROW("Destination coordinate axes are non-orthogonal.");

            if (const auto rot = get_rotation(dst_row_dir, CardinalDirection::EAST))
            {
                src_row_dir = rotate(src_row_dir, rot);
                src_col_dir = rotate(src_col_dir, rot);
            }

            if (is_flipped(dst_row_dir, dst_col_dir))
                src_col_dir = rotate(src_col_dir, 2);

            switch (src_row_dir)
            {
            case CardinalDirection::NORTH:
                if (src_col_dir == CardinalDirection::EAST)
                    return Chorasmia::Index2DMode::COLUMNS_REVERSED_ORDER;
                else
                    return Chorasmia::Index2DMode::REVERSED_COLUMNS_REVERSED_ORDER;
            case CardinalDirection::EAST:
                if (src_col_dir == CardinalDirection::NORTH)
                    return Chorasmia::Index2DMode::ROWS_REVERSED_ORDER;
                else
                    return Chorasmia::Index2DMode::ROWS;
            case CardinalDirection::SOUTH:
                if (src_col_dir == CardinalDirection::EAST)
                    return Chorasmia::Index2DMode::COLUMNS;
                else
                    return Chorasmia::Index2DMode::REVERSED_COLUMNS;
            case CardinalDirection::WEST:
                if (src_col_dir == CardinalDirection::NORTH)
                    return Chorasmia::Index2DMode::REVERSED_ROWS_REVERSED_ORDER;
                else
                    return Chorasmia::Index2DMode::REVERSED_ROWS;
            default:
                GRIDLIB_THROW("Invalid source coordinate axes.");
            }
        }

        CardinalDirection get_cardinal_direction(const Xyz::Vector2D& v)
        {
            constexpr auto PI = Xyz::Constants<double>::PI;
            const auto a = Xyz::get_ccw_angle<double>(v, {-1, 1});
            switch (int(floor(2 * a / PI)))
            {
            case 0:
                return CardinalDirection::NORTH;
            case 1:
                return CardinalDirection::EAST;
            case 2:
                return CardinalDirection::SOUTH;
            default:
                return CardinalDirection::WEST;
            }
        }
    }

    Chorasmia::Index2DMode
    get_index_mode_for_top_left_origin(const IGrid& grid)
    {
        auto rv = grid.spatial_data().row_axis();
        auto cv = grid.spatial_data().column_axis();
        const auto r_dir = get_cardinal_direction(Xyz::Vector2D(rv[0], rv[1]));
        const auto c_dir = get_cardinal_direction(Xyz::Vector2D(cv[0], cv[1]));
        return get_index_2d_mode(r_dir, c_dir,
                                 CardinalDirection::EAST,
                                 CardinalDirection::SOUTH);
    }
}
