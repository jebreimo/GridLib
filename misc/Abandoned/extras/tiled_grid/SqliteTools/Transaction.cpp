#include "Transaction.h"

#include "Execute.h"
#include "SqliteException.h"

namespace SqliteTools
{
    namespace
    {
        constexpr char BEGIN_TRANSACTION_SQL[] = "BEGIN TRANSACTION";
        constexpr char COMMIT_SQL[] = "COMMIT";
        constexpr char ROLLBACK_SQL[] = "ROLLBACK";
    }

    Transaction::Transaction(sqlite3* db)
        : db_(db)
    {
        const auto [rc, str]  = execute(db, BEGIN_TRANSACTION_SQL);
        if (rc != SQLITE_OK)
        {
            db_ = nullptr;
            SQLITETOOLS_THROW("Failed to begin transaction: " + str);
        }
    }

    Transaction::~Transaction()
    {
        rollback();
    }

    void Transaction::commit()
    {
        if (!db_)
            return;

        auto [rc, str] = execute(db_, COMMIT_SQL);
        db_ = nullptr;
        if (rc != SQLITE_OK)
        {
            SQLITETOOLS_THROW("Failed to commit transaction: " + str);
        }
    }

    void Transaction::rollback()
    {
        if (!db_)
            return;

        auto [rc, str] = execute(db_, ROLLBACK_SQL);
        db_ = nullptr;
        if (rc != SQLITE_OK)
        {
            SQLITETOOLS_THROW("Failed to rollback transaction: " + str);
        }
    }
}
