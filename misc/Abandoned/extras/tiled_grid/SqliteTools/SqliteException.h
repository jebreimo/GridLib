#pragma once
#include <stdexcept>
#include <string>

typedef struct sqlite3 sqlite3;

namespace SqliteTools
{
    class SqliteException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    [[nodiscard]]
    std::string get_error_message(sqlite3* db);
}

#define SQLITETOOLS_IMPL_THROW_3(file, line, msg) \
      throw ::SqliteTools::SqliteException(file ":" #line ": " msg)

#define SQLITETOOLS_IMPL_THROW_2(file, line, msg) \
      SQLITETOOLS_IMPL_THROW_3(file, line, msg)

#define SQLITETOOLS_THROW(msg) \
      SQLITETOOLS_IMPL_THROW_2(__FILE__, __LINE__, msg)
