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

    FortranReader::FortranReader(std::istream& stream, size_t bufferSize)
        : m_Buffer(bufferSize),
          m_Stream(&stream)
    {}

    std::optional<char> FortranReader::read_char()
    {
        auto str = read_string(1);
        if (str.empty())
            return {};
        return str[0];
    }

    std::string_view FortranReader::read_string(size_t size, bool trimSpaces)
    {
        while (m_Str.size() < size && fill_buffer(size))
            continue;
        if (size > m_Str.size())
            GRIDLIB_THROW("End of file reached.");
        size = std::min(size, m_Str.size());
        auto result = m_Str.substr(0, size);
        m_Str = m_Str.substr(size);
        return trimSpaces ? trim(result) : result;
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
        if (!m_Stream || !*m_Stream)
            return false;
        if (m_Str.size() >= size)
            return true;
        if (m_Str.data() != m_Buffer.data())
        {
            std::copy(m_Str.begin(), m_Str.end(),
                      m_Buffer.begin());
            m_Str = {m_Buffer.data(), m_Str.size()};
        }
        if (m_Buffer.size() < size)
        {
            auto prevSize = m_Buffer.size();
            m_Buffer.resize(size);
            m_Str = {m_Buffer.data(), prevSize};
        }
        auto bytesToRead = m_Buffer.size() - m_Str.size();
        m_Stream->read(m_Buffer.data() + m_Str.size(), bytesToRead);
        auto bytesRead = size_t(m_Stream->gcount());
        m_Buffer.resize(m_Str.size() + bytesRead);
        m_Str = {m_Buffer.data(), m_Buffer.size()};
        return bytesRead != 0;
    }

    size_t FortranReader::remaining_buffer_size() const
    {
        return m_Str.size();
    }

    void FortranReader::skip(size_t size)
    {
        if (size <= m_Str.size())
        {
            m_Str = m_Str.substr(size);
            return;
        }

        size -= m_Str.size();
        m_Str = {};
        auto start = std::streamoff(m_Stream->tellg());
        m_Stream->seekg(size, std::ios::cur);
        auto end = std::streamoff(m_Stream->tellg());
        if (size_t(end - start) != size)
            GRIDLIB_THROW("End of file reached.");
    }

    bool FortranReader::seek(std::streamoff pos, std::ios_base::seekdir dir)
    {
        if (dir == std::ios_base::cur)
        {
            if (pos <= m_Str.size())
            {
                m_Str = m_Str.substr(pos);
                return true;
            }
            pos -= m_Str.size();
        }
        m_Str = {};
        return bool(m_Stream->seekg(pos));
    }

    std::streamsize FortranReader::tell() const
    {
        return std::streamsize(m_Stream->tellg()) - m_Str.size();
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
