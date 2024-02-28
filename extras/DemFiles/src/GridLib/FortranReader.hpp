//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-09-24.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <ios>

namespace GridLib
{
    class FortranReader
    {
    public:
        FortranReader();

        explicit FortranReader(std::istream& stream,
                               size_t buffer_size = 8192);

        std::string_view read_string(size_t size, bool trim_spaces = true);

        std::optional<char> read_char();

        std::optional<int8_t> read_int8(size_t size);

        std::optional<int16_t> read_int16(size_t size);

        std::optional<int32_t> read_int32(size_t size);

        std::optional<float> read_float32(size_t size);

        std::optional<double> read_float64(size_t size);

        void skip(size_t size);

        [[nodiscard]]
        size_t remaining_buffer_size() const;

        bool fill_buffer(size_t size);

        bool seek(std::streamoff pos, std::ios_base::seekdir dir);

        [[nodiscard]]
        std::streamsize tell() const;
    private:
        template <typename T>
        std::optional<T> read_int(size_t size);

        template <typename T>
        std::optional<T> read_float(size_t size);

        std::istream* stream_ = nullptr;
        std::string_view str_;
        std::vector<char> buffer_;
    };
}
