//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-11-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "TemporaryFile.hpp"

#include <random>

#include "GridLib/GridLibException.hpp"

namespace GridLib
{
    namespace
    {
        std::filesystem::path make_temp_filename()
        {
            static unsigned counter = std::random_device{}();
            return std::filesystem::temp_directory_path() / ("grid_" + std::to_string(counter++) + ".dat");
        }

        std::pair<std::filesystem::path, std::fstream> make_temp_file()
        {
            for (int i = 0; i < 10; ++i)
            {
                auto temp_filename = make_temp_filename();
                if (!std::filesystem::exists(temp_filename))
                {
                    std::fstream file(temp_filename,
                                      std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
                    if (file)
                        return {temp_filename, std::move(file)};
                }
            }

            GRIDLIB_THROW("Unable to make temporary file");
        }
    }

    TemporaryFile::TemporaryFile()
    {
        auto [path, stream] = make_temp_file();
        file_path_ = std::move(path);
        file_stream_ = std::move(stream);
    }

    TemporaryFile::~TemporaryFile()
    {
        close();
    }

    void TemporaryFile::close()
    {
        if (file_stream_.is_open())
        {
            file_stream_.close();
            std::filesystem::remove(file_path_);
            file_path_.clear();
        }
    }
}
