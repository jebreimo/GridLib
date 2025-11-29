#pragma once
#include <filesystem>
#include <string>
#include <sqlite3.h>

namespace SqliteTools
{
    class Connection
    {
    public:
        Connection();

        explicit Connection(sqlite3* db) noexcept;

        /**
         * @brief Opens or creates database @a path_utf8 depending on the value
         *     of @a flags.
         *
         * @param path  the path to a sqlite database
         * @param flags  see the sqlite3_open documentation for a
         *     comprehensive list of values.
         */
        Connection(const std::filesystem::path& path,
                   int flags,
                   const char* z_vfs = nullptr);

        Connection(const Connection& other) = delete;

        Connection(Connection&& other) noexcept;

        ~Connection();

        Connection& operator=(const Connection& other) = delete;

        Connection& operator=(Connection&& other) noexcept;

        explicit operator bool() const;

        void close();

        operator sqlite3*() const;

    private:
        sqlite3* db_ = nullptr;
    };

    Connection create(const std::filesystem::path& path);

    /**
     * @brief Opens database @a path in read/write mode.
      */
    Connection open(const std::filesystem::path& path);

    /**
     * @brief Opens database @a path in read/write mode, creating it if the
     *     file doesn't already exist.
     */
    Connection open_or_create(const std::filesystem::path& path);

    /**
     * @brief Opens database @a path in read-only mode.
     */
    Connection open_read_only(const std::filesystem::path& path);
}
