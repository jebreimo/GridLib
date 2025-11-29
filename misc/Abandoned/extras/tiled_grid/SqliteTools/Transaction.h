#pragma once

#include <sqlite3.h>

namespace SqliteTools
{
    class Transaction
    {
    public:
        explicit Transaction(sqlite3* db);

        ~Transaction();

        void commit();

        void rollback();

    private:
        sqlite3* db_;
    };
}
