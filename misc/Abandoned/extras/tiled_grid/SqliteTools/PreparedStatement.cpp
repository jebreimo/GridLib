#include "PreparedStatement.h"

#include <cstdlib>
#include <string>
#include "SqliteException.h"

#define CHECK_SQLITE_RESULT_CODE(rc) \
    if ((rc) != SQLITE_OK) \
    { \
        SQLITETOOLS_THROW(+ get_error_message(sqlite3_db_handle(statement_))); \
    }


#define CHECK_SQLITE_RESULT_VALUE(value, suspect_value) \
    if ((value) == (suspect_value)) \
    { \
        sqlite3* connection_ = sqlite3_db_handle(statement_); \
        if (int ec_ = sqlite3_errcode(connection_); ec_ != SQLITE_OK) \
        { \
            SQLITETOOLS_THROW(+ get_error_message(sqlite3_db_handle(statement_))); \
        } \
    } \

namespace SqliteTools
{
    PreparedStatement::PreparedStatement(sqlite3* db,
                                         std::string_view statement)
        : statement_(nullptr)
    {
        const char* next_statement;
        const auto rc = sqlite3_prepare_v2(db,
                                           statement.data(),
                                           int(statement.size()),
                                           &statement_,
                                           &next_statement);
        CHECK_SQLITE_RESULT_CODE(rc);
    }

    PreparedStatement::~PreparedStatement()
    {
        finalize();
    }

    PreparedStatement::PreparedStatement(PreparedStatement&& other) noexcept
        : statement_(other.statement_)
    {
        other.statement_ = nullptr;
    }

    PreparedStatement&
    PreparedStatement::operator=(PreparedStatement&& other) noexcept
    {
        finalize();
        std::swap(statement_, other.statement_);
        return *this;
    }

    bool PreparedStatement::step()
    {
        switch (auto rc = sqlite3_step(statement_))
        {
        case SQLITE_ROW:
            return true;
        case SQLITE_DONE:
            return false;
        default:
            SQLITETOOLS_THROW(+ std::string(sqlite3_errstr(rc)));
        }
    }

    void PreparedStatement::reset()
    {
        CHECK_SQLITE_RESULT_CODE(sqlite3_reset(statement_));
    }

    void PreparedStatement::finalize()
    {
        auto rc = sqlite3_finalize(statement_);
        statement_ = nullptr;
        CHECK_SQLITE_RESULT_CODE(rc);
    }

    void PreparedStatement::bind(int index, int value)
    {
        CHECK_SQLITE_RESULT_CODE(sqlite3_bind_int(statement_, index, value));
    }

    void PreparedStatement::bind(int index, double value)
    {
        CHECK_SQLITE_RESULT_CODE(sqlite3_bind_double(statement_, index, value));
    }

    void PreparedStatement::bind(int index, int64_t value)
    {
        CHECK_SQLITE_RESULT_CODE(sqlite3_bind_int64(statement_, index, value));
    }

    namespace
    {
        auto get_delete_func(const LifetimeManagement& lifetime) -> void (*)(void*)
        {
            if (std::holds_alternative<void (*)(void*)>(lifetime))
                return std::get<void (*)(void*)>(lifetime);

            const auto value = std::get<Lifetime>(lifetime);
            if (value == Lifetime::STATIC)
                return SQLITE_STATIC;
            if (value == Lifetime::TRANSIENT)
                return SQLITE_TRANSIENT;

            SQLITETOOLS_THROW("Unknown lifetime type");
        }
    }

    void PreparedStatement::bind(int index, std::string_view value,
                                 LifetimeManagement lifetime)
    {
        if (value.size() > INT_MAX)
            SQLITETOOLS_THROW("string length is too large");

        const auto rc = sqlite3_bind_text(statement_, index, value.data(),
                                          static_cast<int>(value.size()),
                                          get_delete_func(lifetime));
        CHECK_SQLITE_RESULT_CODE(rc);
    }

    void PreparedStatement::bind(int index, const void* blob, size_t size,
                                 LifetimeManagement lifetime)
    {
        if (size > INT_MAX)
            SQLITETOOLS_THROW("blob size is too large");

        const auto rc = sqlite3_bind_blob(statement_, index, blob,
                                          static_cast<int>(size),
                                          get_delete_func(lifetime));
        CHECK_SQLITE_RESULT_CODE(rc);
    }

    int PreparedStatement::column_count() const
    {
        return sqlite3_column_count(statement_);
    }

    int PreparedStatement::column_type(int index) const
    {
        return sqlite3_column_type(statement_, index);
    }

    double PreparedStatement::double_column(int index) const
    {
        return sqlite3_column_double(statement_, index);
    }

    int PreparedStatement::int_column(int index) const
    {
        return sqlite3_column_int(statement_, index);
    }

    int64_t PreparedStatement::int64_column(int index) const
    {
        return sqlite3_column_int64(statement_, index);
    }

    std::string_view PreparedStatement::string_column(int index) const
    {
        const auto str = reinterpret_cast<const char*>(
            sqlite3_column_text(statement_, index));
        CHECK_SQLITE_RESULT_VALUE(str, nullptr);
        const auto size = static_cast<size_t>(sqlite3_column_bytes(statement_, index));
        return std::string_view(str, size); // NOLINT(*-return-braced-init-list)
    }

    std::pair<const void*, size_t> PreparedStatement::blob_column(int index) const
    {
        const auto blob = sqlite3_column_blob(statement_, index);
        CHECK_SQLITE_RESULT_VALUE(blob, nullptr);
        const auto size = static_cast<size_t>(sqlite3_column_bytes(statement_, index));
        return {blob, size};
    }
}
