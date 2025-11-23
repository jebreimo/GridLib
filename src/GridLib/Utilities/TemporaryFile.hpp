//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-11-10.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <fstream>
#include <filesystem>

namespace GridLib
{
    class TemporaryFile
    {
    public:
        TemporaryFile();

        ~TemporaryFile();

        std::fstream& stream()
        {
            return file_stream_;
        }

        std::filesystem::path path() const
        {
            return file_path_;
        }

        void close();
    private:
        std::filesystem::path file_path_;
        std::fstream file_stream_;
    };
}
