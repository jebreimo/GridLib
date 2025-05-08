//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-05-04.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <Xyz/Matrix.hpp>

namespace GridLib
{
    class ModelMatrix
    {
    public:
        explicit ModelMatrix(const Xyz::Matrix4D& matrix)
            : matrix_(matrix)
        {
        }

        [[nodiscard]]
        const Xyz::Matrix4D& matrix() const
        {
            return matrix_;
        }

        [[nodiscard]]
        Xyz::Vector3D position() const
        {
            return {matrix_[{0, 3}], matrix_[{1, 3}], matrix_[{2, 3}]};
        }

        [[nodiscard]]
        Xyz::Vector3D col_axis() const
        {
            return {matrix_[{0, 0}], matrix_[{0, 1}], matrix_[{0, 2}]};
        }

        [[nodiscard]]
        Xyz::Vector3D row_axis() const
        {
            return {matrix_[{1, 0}], matrix_[{1, 1}], matrix_[{1, 2}]};
        }

        [[nodiscard]]
        Xyz::Vector3D vertical_axis() const
        {
            return {matrix_[{2, 0}], matrix_[{2, 1}], matrix_[{2, 2}]};
        }
    private:
        const Xyz::Matrix4D& matrix_;
    };

    class ModelMatrixBuilder
    {
    public:
        [[nodiscard]]
        const Xyz::Matrix4D& matrix() const
        {
            return matrix_;
        }

        ModelMatrixBuilder& position(const Xyz::Vector3D& v)
        {
            matrix_[{0, 3}] = v[0];
            matrix_[{1, 3}] = v[1];
            matrix_[{2, 3}] = v[2];
            return *this;
        }

        ModelMatrixBuilder& col_axis(const Xyz::Vector3D& v)
        {
            matrix_[{0, 0}] = v[0];
            matrix_[{0, 1}] = v[1];
            matrix_[{0, 2}] = v[2];
            return *this;
        }

        ModelMatrixBuilder& row_axis(const Xyz::Vector3D& v)
        {
            matrix_[{1, 0}] = v[0];
            matrix_[{1, 1}] = v[1];
            matrix_[{1, 2}] = v[2];
            return *this;
        }

        ModelMatrixBuilder& vertical_axis(const Xyz::Vector3D& v)
        {
            matrix_[{2, 0}] = v[0];
            matrix_[{2, 1}] = v[1];
            matrix_[{2, 2}] = v[2];
            return *this;
        }
    private:
        Xyz::Matrix4D matrix_ = Xyz::Matrix4D::identity();
    };
}
