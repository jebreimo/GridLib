//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-10-06.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <iosfwd>
#include <memory>
#include <optional>
#include <vector>
#include "RecordA.hpp"
#include "RecordC.hpp"
#include "RecordB.hpp"

namespace GridLib
{
    class DemReader
    {
    public:
        explicit DemReader(std::istream& stream);

        DemReader(DemReader&& rhs) noexcept;

        ~DemReader();

        DemReader& operator=(DemReader&& rhs) noexcept;

        [[nodiscard]]
        const RecordA& record_a() const;

        [[nodiscard]]
        const std::optional<RecordC>& record_c() const;

        [[nodiscard]]
        std::optional<RecordB> next_record_b();
    private:
        void read_record_a();

        [[nodiscard]]
        std::optional<RecordB> read_record_b();

        void read_record_c();

        void move_to_first_record_b();

        struct Data;
        std::unique_ptr<Data> data_;
    };
}
