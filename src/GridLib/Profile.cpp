//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "GridLib/Profile.hpp"
#include <cmath>
#include <Chorasmia/ArrayView2DAlgorithms.hpp>
#include <Xyz/LineClipping.hpp>
#include <Xyz/InvertMatrix.hpp>

namespace GridLib
{
    namespace
    {
        [[nodiscard]]
        Xyz::RectangleD to_rectangle(const IGrid& grid)
        {
            auto width = double(grid.row_count() - 1);
            auto height = double(grid.col_count() - 1);
            return {{0, 0}, {width, height}};
        }

        [[nodiscard]]
        Xyz::Vector3D get_point(const IGrid& grid,
                                const Xyz::Vector2D& pos)
        {
            return make_vector3(pos, get_elevation(grid, pos));
        }

        [[nodiscard]]
        Xyz::Matrix4D get_transform(const IGrid& grid)
        {
            return grid.model().matrix;
        }

        [[nodiscard]]
        Xyz::Vector2D transform_point(const Xyz::Matrix4D& transform,
                                      const Xyz::Vector2D& point)
        {
            const auto v4 = Xyz::make_vector4(point[0], point[1], 0.0, 1.0);
            auto result = transform * v4;
            return {result[0], result[1]};
        }

        Xyz::Vector3D transform_point(const Xyz::Matrix4D& transform,
                                      const Xyz::Vector3D& point)
        {
            const auto v4 = make_vector4(point, 1.0);
            auto result = transform * v4;
            return {result[0], result[1], result[2]};
        }
    }

    std::vector<Xyz::Vector3D>
    make_profile(const IGrid& grid,
                 const Xyz::Vector2D& from,
                 const Xyz::Vector2D& to,
                 size_t steps)
    {
        Xyz::LineSegment line(from, to);
        const auto clip = get_clipping_positions(to_rectangle(grid), line);
        if (!clip)
            return {};

        line = make_line_segment(line, clip->first, clip->second);

        const auto step_size = (to - from) / double(steps);
        const auto first = std::ceil(clip->first * double(steps));
        const auto last = std::floor(clip->second * double(steps));
        const auto min_step_length = 1e-3 / double(steps);

        std::vector<Xyz::Vector3D> result;
        if (std::abs(first / double(steps) - clip->first) > min_step_length)
            result.push_back(get_point(grid, line.start()));

        for (auto i = size_t(first), n = size_t(last); i <= n; ++i)
        {
            auto pos = from + double(i) * step_size;
            result.push_back(get_point(grid, pos));
        }

        if (std::abs(clip->second - last / double(steps)) > min_step_length)
            result.push_back(get_point(grid, line.end()));

        return result;
    }

    ProfileMaker::ProfileMaker(const IGrid& grid)
        : grid_(grid.subgrid(0, 0)),
          transform_(get_transform(grid_)),
          inverse_transform_(invert(transform_))
    {}

    std::vector<Xyz::Vector3D>
    ProfileMaker::make_profile(const Xyz::Vector2D& from,
                               const Xyz::Vector2D& to,
                               size_t steps) const
    {
        const auto from2 = transform_point(inverse_transform_, from);
        const auto to2 = transform_point(inverse_transform_, to);
        auto profile = GridLib::make_profile(grid_, from2, to2, steps);
        for (auto& point : profile)
            point = transform_point(transform_, point);
        return profile;
    }
}
