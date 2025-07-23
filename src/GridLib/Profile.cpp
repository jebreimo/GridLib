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

namespace GridLib
{
    namespace
    {
        Xyz::Matrix4D get_clip_transform(const IGrid& grid)
        {
            const auto model_rect = get_bounds(grid);
            return Xyz::get_clip_transform(model_rect)
                   * Xyz::make_projection_matrix(get_plane(model_rect));
        }
    }

    std::vector<Xyz::Vector3D> make_profile(const IGrid& grid,
                                            const Xyz::Vector3D& from,
                                            const Xyz::Vector3D& to,
                                            size_t segments)
    {
        ProfileMaker profile_maker(grid);
        return profile_maker.make_profile(from, to, segments);
    }

    ProfileMaker::ProfileMaker(const IGrid& grid)
        : interpolator_(grid),
          clipper_(get_clip_transform(grid))
    {
    }

    std::vector<Xyz::Vector3D>
    ProfileMaker::make_profile(const Xyz::Vector3D& from,
                               const Xyz::Vector3D& to,
                               size_t segments) const
    {
        if (segments == 0)
            return {};

        const auto line = clipper_.clip({from, to});
        if (!line)
        {
            // The line segment is outside the grid.
            return {};
        }

        const auto total_length = get_length(to - from);

        size_t step0 = 0;
        if (line->start != from)
        {
            step0 = static_cast<size_t>(
                std::ceil((get_length(line->start - from) / total_length) * static_cast<double>(segments)));
        }

        size_t step1 = segments;
        if (line->end != to)
        {
            step1 = static_cast<size_t>(
                std::floor((get_length(line->end - from) / total_length) * static_cast<double>(segments)));
        }

        std::vector<Xyz::Vector3D> result;
        if (step0 != 0)
        {
            if (const auto p = interpolator_.at_model_pos(line->start))
                result.push_back(*p);
        }

        for (size_t i = step0; i <= step1; ++i)
        {
            auto pos = from + (to - from) * double(i) / double(segments);
            if (auto p = interpolator_.at_model_pos(pos))
                result.push_back(*p);
        }

        if (step1 != segments)
        {
            auto p = interpolator_.at_model_pos(line->end);
            if (p)
                result.push_back(*p);
        }

        return result;
    }
}
