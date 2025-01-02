//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-09-24.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "FortranReader.hpp"

#include <algorithm>
#include <istream>
#include "GridLib/GridLibException.hpp"
#include "ParseNumber.hpp"

namespace GridLib
{
    namespace
    {
        std::string_view trim(std::string_view str)
        {
            auto i = std::find_if(str.begin(), str.end(),
                                  [](auto c) {return c != ' ';});
            if (i == str.end())
                return str.substr(str.size());
            auto j = std::find_if(str.rbegin(), str.rend(),
                                  [](auto c) {return c != ' ';});
            return str.substr(std::distance(str.begin(), i),
                              std::distance(i, j.base()));
        }
    }

    FortranReader::FortranReader() = default;

    FortranReader::FortranReader(std::istream& stream, size_t buffer_size)
        : buffer_(buffer_size),
          stream_(&stream)
    {}

    std::optional<char> FortranReader::read_char()
    {
        auto str = read_string(1);
        if (str.empty())
            return {};
        return str[0];
    }

    std::string_view FortranReader::read_string(size_t size, bool trim_spaces)
    {
        while (str_.size() < size && fill_buffer(size))
            continue;
        if (size > str_.size())
            GRIDLIB_THROW("End of file reached.");
        size = std::min(size, str_.size());
        auto result = str_.substr(0, size);
        str_ = str_.substr(size);
        return trim_spaces ? trim(result) : result;
    }

    std::optional<int8_t> FortranReader::read_int8(size_t size)
    {
        return read_int<int8_t>(size);
    }

    std::optional<int16_t> FortranReader::read_int16(size_t size)
    {
        return read_int<int16_t>(size);
    }

    std::optional<int32_t> FortranReader::read_int32(size_t size)
    {
        return read_int<int32_t>(size);
    }

    std::optional<float> FortranReader::read_float32(size_t size)
    {
        return read_float<float>(size);
    }

    std::optional<double> FortranReader::read_float64(size_t size)
    {
        return read_float<double>(size);
    }

    bool FortranReader::fill_buffer(size_t size)
    {
        if (!stream_ || !*stream_)
            return false;
        if (str_.size() >= size)
            return true;
        if (str_.data() != buffer_.data())
        {
            std::copy(str_.begin(), str_.end(),
                      buffer_.begin());
            str_ = {buffer_.data(), str_.size()};
        }
        if (buffer_.size() < size)
        {
            auto prevSize = buffer_.size();
            buffer_.resize(size);
            str_ = {buffer_.data(), prevSize};
        }
        auto bytes_to_read = buffer_.size() - str_.size();
        stream_->read(buffer_.data() + str_.size(),
                      std::streamsize(bytes_to_read));
        auto bytes_read = size_t(stream_->gcount());
        buffer_.resize(str_.size() + bytes_read);
        str_ = {buffer_.data(), buffer_.size()};
        return bytes_read != 0;
    }

    size_t FortranReader::remaining_buffer_size() const
    {
        return str_.size();
    }

    void FortranReader::skip(size_t size)
    {
        if (size <= str_.size())
        {
            str_ = str_.substr(size);
            return;
        }

        size -= str_.size();
        str_ = {};
        auto start = std::streamoff(stream_->tellg());
        stream_->seekg(std::ios::off_type(size), std::ios::cur);
        auto end = std::streamoff(stream_->tellg());
        if (size_t(end - start) != size)
            GRIDLIB_THROW("End of file reached.");
    }

    bool FortranReader::seek(std::streamoff pos, std::ios_base::seekdir dir)
    {
        if (dir == std::ios_base::cur)
        {
            if (pos <= str_.size())
            {
                str_ = str_.substr(pos);
                return true;
            }
            pos -= std::streamoff(str_.size());
        }
        str_ = {};
        return bool(stream_->seekg(pos));
    }

    std::streamsize FortranReader::tell() const
    {
        return std::streamsize(stream_->tellg()) - std::streamsize(str_.size());
    }

    template <typename T>
    std::optional<T> FortranReader::read_int(size_t size)
    {
        auto str = read_string(size);
        if (str.empty())
            return {};
        T n;
        if (!parse(str, n))
            GRIDLIB_THROW("Invalid integer");
        return std::optional<T>(n);
    }

    template <typename T>
    std::optional<T> FortranReader::read_float(size_t size)
    {
        auto str = read_string(size);
        if (str.empty())
            return {};
        T n;
        if (!parse(str, n))
            GRIDLIB_THROW("Invalid integer");
        return std::optional<T>(n);
    }
}
