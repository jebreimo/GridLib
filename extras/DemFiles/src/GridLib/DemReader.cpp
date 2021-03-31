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
        : m_Data(std::make_unique<Data>(stream))
    {
        read_record_a();
        read_record_c();
    }

    DemReader::DemReader(DemReader&& rhs) noexcept = default;

    DemReader::~DemReader() = default;

    DemReader& DemReader::operator=(DemReader&& rhs) noexcept = default;

    const RecordA& DemReader::record_a() const
    {
        return m_Data->a;
    }

    const std::optional<RecordC>& DemReader::record_c() const
    {
        return m_Data->c;
    }

    std::optional<RecordB> DemReader::next_record_b()
    {
        return read_record_b();
    }

    void DemReader::read_record_a()
    {
        if (!m_Data)
            GRIDLIB_THROW("No input stream.");


        try
        {
            m_Data->a = GridLib::read_record_a(m_Data->reader);
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
        if (!m_Data)
            GRIDLIB_THROW("No input stream.");

        if (!m_Data->reader.fill_buffer(1024))
            return {};

        if (m_Data->a.data_validation_flag.value_or(0) != 0
            && m_Data->reader.fill_buffer(2048)
            && m_Data->reader.remaining_buffer_size() == 1024)
        {
            // We've reached the final 1024 bytes of the file, which contains
            // record type c.
            return {};
        }

        try
        {
            return GridLib::read_record_b(m_Data->reader);
        }
        catch (std::exception& ex)
        {
            GRIDLIB_THROW("Invalid record of type B.\n    "
                          + std::string(ex.what()));
        }
    }

    void DemReader::read_record_c()
    {
        if (m_Data->a.data_validation_flag.value_or(0) == 0)
            return;

        m_Data->reader.seek(-1024, std::ios_base::end);

        try
        {
            m_Data->c = GridLib::read_record_c(m_Data->reader);
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
        m_Data->reader.seek(1024, std::ios_base::beg);
    }
}
