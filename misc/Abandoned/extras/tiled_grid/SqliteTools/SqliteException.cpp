//****************************************************************************
// Copyright © 2025 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2025-08-17.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "SqliteException.h"

#include <sqlite3.h>

namespace SqliteTools
{
    std::string get_error_message(sqlite3* db)
    {
        const auto error = sqlite3_errmsg(db);
        return error ? std::string(error) : std::string();
        // const int ec = sqlite3_errcode(db);
        // if (ec == SQLITE_OK)
        //     return {};
        //
        // std::string msg = "SQLITE " + std::to_string(ec) + ": "
        //                   + sqlite3_errstr(ec);
        // if (const int offset = sqlite3_error_offset(db); offset >= 0)
        //     msg += " (offset: " + std::to_string(offset) + ")";
        //
        // return msg;
    }

}
