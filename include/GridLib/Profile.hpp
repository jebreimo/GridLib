//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include <Xyz/LineClipper.hpp>
#include <Xyz/Vector.hpp>

#include "GridInterpolator.hpp"
#include "GridView.hpp"

namespace GridLib
{
    /**
     * @brief Creates a profile of the grid between two points.
     * @param grid The grid to create the profile from.
     * @param from The starting point of the profile.
     * @param to The end point of the profile.
     * @param segments The number of line segments in the profile.
     * @return A vector with the points in the profile.
     */
    [[nodiscard]] std::vector<Xyz::Vector3D>
    make_profile(const IGrid& grid,
                 const Xyz::Vector3D& from,
                 const Xyz::Vector3D& to,
                 size_t segments);

    class ProfileMaker
    {
    public:
        explicit ProfileMaker(const IGrid& grid);

        /**
         * @brief Creates a profile of the grid between two points.
         * @param from The starting point of the profile.
         * @param to The end point of the profile.
         * @param segments The number of line segments in the profile.
         * @return A vector with the points in the profile.
         */
        [[nodiscard]] std::vector<Xyz::Vector3D>
        make_profile(const Xyz::Vector3D& from,
                     const Xyz::Vector3D& to,
                     size_t segments) const;
    private:
        GridInterpolator interpolator_;
        Xyz::LineClipper<double, 3> clipper_;
    };
}
