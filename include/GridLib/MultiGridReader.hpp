//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-11-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <filesystem>
#include <memory>

#include "Grid.hpp"

namespace GridLib
{
    class Grid;

    using SignedIndex = Chorasmia::Index2D<int64_t>;
    using SignedExtent = Chorasmia::Extent2D<int64_t>;
    struct GridData;

    class MultiGridReader
    {
    public:
        MultiGridReader();

        ~MultiGridReader();

        MultiGridReader(MultiGridReader&& other) noexcept;

        MultiGridReader& operator=(MultiGridReader&& other) noexcept;

        [[nodiscard]] Size size() const;

        void read_grid(const std::filesystem::path& filename);

        [[nodiscard]] bool has_data(const Extent& extent) const;

        [[nodiscard]] Grid get_grid(Extent extent) const;
    private:
        void assert_data() const;

        void assert_compatible_grid(const Grid& grid) const;

        void load_and_copy_grid_data(Grid& result,
                                     const GridData& grid_data,
                                     const SignedExtent& extent) const;

        struct Data;
        std::unique_ptr<Data> data_;
    };
}
