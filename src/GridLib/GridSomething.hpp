//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-04-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Xyz/Matrix.hpp>
#include <Xyz/Vector.hpp>

#include "GridLib/GridMemberTypes.hpp"
#include "GridLib/Unit.hpp"

namespace GridLib
{
    class GridSomething
    {
    public:
        virtual ~GridSomething();

        virtual const Xyz::Vector2D& size() const = 0;

        virtual const Xyz::Matrix3D& axes() const = 0;

        [[nodiscard]]
        size_t row_count() const;

        [[nodiscard]]
        size_t col_count() const;

        [[nodiscard]]
        const Xyz::Vector3D& row_axis() const;

        [[nodiscard]]
        const Xyz::Vector3D& col_axis() const;

        [[nodiscard]]
        const Xyz::Vector3D& vertical_axis() const;

        [[nodiscard]]
        Unit horizontal_unit() const;

        [[nodiscard]]
        Unit vertical_unit() const;

        [[nodiscard]]
        const Coordinates& coordinates() const;

        [[nodiscard]]
        const CoordinateReferenceSystem& reference_system() const;
    };
}
