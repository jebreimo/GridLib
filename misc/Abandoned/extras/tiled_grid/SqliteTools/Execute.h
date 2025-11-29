#pragma once

#include <string>
#include <sqlite3.h>

namespace SqliteTools
{
    struct ErrorResult
    {
        int code = SQLITE_OK;
        std::string msg;

        explicit operator bool() const
        {
            return code != SQLITE_OK;
        }
    };

    namespace detail
    {
        inline ErrorResult do_execute(sqlite3* db,
                                      const char* sql,
                                      int (*callback)(void*, int, char**, char**),
                                      void* context)
        {
            char* tmp_err_msg = nullptr;
            const auto result = sqlite3_exec(db, sql, callback, context, &tmp_err_msg);
            if (tmp_err_msg)
            {
                ErrorResult exec_result(result, std::string(tmp_err_msg));
                sqlite3_free(tmp_err_msg);
                return exec_result;
            }
            return {result, {}};
        }

        template <typename TernaryFunc>
        class CallbackWrapper
        {
        public:
            explicit CallbackWrapper(TernaryFunc func)
                : func_(func)
            {
            }

            ErrorResult execute(sqlite3* db, const char* sql)
            {
                return do_execute(db, sql, &CallbackWrapper::callback, this);
            }

        private:
            static int callback(void* context, int ncols,
                                char** col_values, char** col_names)
            {
                auto instance = static_cast<CallbackWrapper*>(context);
                return instance->func_(ncols, col_values, col_names);
            }

            TernaryFunc func_;
        };
    }

    /**
     * @brief Executes @a sql against the open database connection @a db.
     *
     * This function is a convenience wrapper around sqlite3_exec. Unlike the
     * function it wraps, it allows any * kind of C++ callback function to be
     * invoked, including lambdas.
     *
     * The callback function must have the following signature:
     *     int (int ncols, char** col_values, char** col_names)
     * where `ncols` is the number of columns in the row, col_values is an
     * array of `ncols` column values, and col_names is an array of `ncols`
     * column names.
     * The callback should return SQLITE_OK unless it wants to abort further
     * execution of any statements in @a SQL.
     *
     * @param db A database opened with sqlite3_open_v2 or equivalent.
     * @param sql The SQL statements that will be executed. Separate multiple
     *     statements with semicolons.
     * @param callback A callback function that will be called once for
     *     each result row that any of the statements in @a SQL produce.
     * @return The return value from sqlite3_exec.
     */
    template <typename TernaryFunc>
    ErrorResult execute(sqlite3* db,
                        const char* sql,
                        TernaryFunc callback)
    {
        detail::CallbackWrapper<TernaryFunc> wrapper(callback);
        return wrapper.execute(db, sql);
    }

    /**
     * @brief Executes @a SQL against the open database connection @a db.
     * @param db A database opened with sqlite3_open_v2 or equivalent.
     * @param sql The SQL statements that will be executed. Separate multiple
     *     statements with semicolons.
     */
    inline ErrorResult execute(sqlite3* db,
                               const char* sql)
    {
        return detail::do_execute(db, sql, nullptr, nullptr);
    }
}
