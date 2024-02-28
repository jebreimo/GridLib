//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-10-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "DemReader.hpp"

#include "RecordA.hpp"
#include "RecordB.hpp"
#include "RecordC.hpp"
#include "GridLib/GridLibException.hpp"
#include "FortranReader.hpp"

namespace GridLib
{
    struct DemReader::Data
    {
        explicit Data(std::istream& stream)
            : reader(stream)
        {}

        FortranReader reader;
        RecordA a;
        std::optional<RecordC> c;
    };

    DemReader::DemReader(std::istream& stream)
        : data_(std::make_unique<Data>(stream))
    {
        read_record_a();
        read_record_c();
    }

    DemReader::DemReader(DemReader&& rhs) noexcept = default;

    DemReader::~DemReader() = default;

    DemReader& DemReader::operator=(DemReader&& rhs) noexcept = default;

    const RecordA& DemReader::record_a() const
    {
        return data_->a;
    }

    const std::optional<RecordC>& DemReader::record_c() const
    {
        return data_->c;
    }

    std::optional<RecordB> DemReader::next_record_b()
    {
        return read_record_b();
    }

    void DemReader::read_record_a()
    {
        if (!data_)
            GRIDLIB_THROW("No input stream.");


        try
        {
            data_->a = GridLib::read_record_a(data_->reader);
        }
        catch (std::exception& ex)
        {
            GRIDLIB_THROW("The stream doesn't contain"
                          " a valid record of type A.\n    "
                          + std::string(ex.what()));
        }
    }

    std::optional<RecordB> DemReader::read_record_b()
    {
        if (!data_)
            GRIDLIB_THROW("No input stream.");

        if (!data_->reader.fill_buffer(1024))
            return {};

        if (data_->a.data_validation_flag.value_or(0) != 0
            && data_->reader.fill_buffer(2048)
            && data_->reader.remaining_buffer_size() == 1024)
        {
            // We've reached the final 1024 bytes of the file, which contains
            // record type c.
            return {};
        }

        try
        {
            return GridLib::read_record_b(data_->reader);
        }
        catch (std::exception& ex)
        {
            GRIDLIB_THROW("Invalid record of type B.\n    "
                          + std::string(ex.what()));
        }
    }

    void DemReader::read_record_c()
    {
        if (data_->a.data_validation_flag.value_or(0) == 0)
            return;

        data_->reader.seek(-1024, std::ios_base::end);

        try
        {
            data_->c = GridLib::read_record_c(data_->reader);
        }
        catch (std::exception& ex)
        {
            GRIDLIB_THROW("The stream doesn't contain"
                          " a valid record of type C.\n    "
                          + std::string(ex.what()));
        }

        move_to_first_record_b();
    }

    void DemReader::move_to_first_record_b()
    {
        data_->reader.seek(1024, std::ios_base::beg);
    }
}
