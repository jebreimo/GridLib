//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-04-24.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "GridLib/GridModel.hpp"

namespace GridLib
{
    class IGridMetadata
    {
    public:
        virtual ~IGridMetadata() = default;

        [[nodiscard]]
        virtual size_t row_count() const = 0;

        [[nodiscard]]
        virtual size_t col_count() const = 0;

        [[nodiscard]]
        virtual const Xyz::Vector2D& model_tie_point() const = 0;

        [[nodiscard]]
        virtual const GridModel& model() const = 0;
    };
}
