//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-02-16.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include <Xyz/Matrix.hpp>
#include <Xyz/Vector.hpp>
#include "GridView.hpp"

namespace GridLib
{
    [[nodiscard]] std::vector<Xyz::Vector3D>
    make_profile(const IGrid& grid,
                 const Xyz::Vector2D& from,
                 const Xyz::Vector2D& to,
                 size_t steps);

    class ProfileMaker
    {
    public:
        explicit ProfileMaker(const IGrid& grid);

        [[nodiscard]] std::vector<Xyz::Vector3D>
        make_profile(const Xyz::Vector2D& from,
                     const Xyz::Vector2D& to,
                     size_t steps) const;
    private:
        GridView grid_;
        const Xyz::Matrix4D transform_;
        const Xyz::Matrix4D inverse_transform_;
    };
}
