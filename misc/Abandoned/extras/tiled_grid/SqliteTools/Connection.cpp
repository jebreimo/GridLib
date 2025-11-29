#include "Connection.h"

#include "SqliteException.h"

namespace SqliteTools
{
    Connection::Connection() = default;

    Connection::Connection(sqlite3* db) noexcept
        : db_(db)
    {
    }

    Connection::Connection(const std::filesystem::path& path,
                           int flags,
                           const char* z_vfs)
    {
        auto path_utf8 = path.u8string();
        const auto path_cstr = reinterpret_cast<const char*>(path_utf8.c_str());
        if (int result = sqlite3_open_v2(path_cstr, &db_, flags, z_vfs))
        {
            sqlite3_close(db_);
            db_ = nullptr;
            std::string error_msg = "Failed to open database '"
                                    + path.string() + "': ";
            if (const auto msg = sqlite3_errstr(result); msg != nullptr)
                error_msg.append(msg);
            else
                error_msg += "Unknown error";
            SQLITETOOLS_THROW(+ error_msg);
        }
    }

    Connection::Connection(Connection&& other) noexcept
        : db_(other.db_)
    {
        other.db_ = nullptr;
    }

    Connection& Connection::operator=(Connection&& other) noexcept
    {
        close();
        db_ = other.db_;
        other.db_ = nullptr;
        return *this;
    }

    Connection::~Connection()
    {
        close();
    }

    Connection::operator bool() const
    {
        return db_ != nullptr;
    }

    void Connection::close()
    {
        if (db_)
        {
            sqlite3_close(db_);
            db_ = nullptr;
        }
    }

    Connection::operator sqlite3*() const
    {
        return db_;
    }

    Connection create(const std::filesystem::path& path)
    {
        if (std::filesystem::exists(path))
            std::filesystem::remove(path);
        return {path, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI};
    }

    Connection open(const std::filesystem::path& path)
    {
        return {path, SQLITE_OPEN_READWRITE | SQLITE_OPEN_URI};
    }

    Connection open_or_create(const std::filesystem::path& path)
    {
        return {path, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI};
    }

    Connection open_read_only(const std::filesystem::path& path)
    {
        return {path, SQLITE_OPEN_READONLY | SQLITE_OPEN_URI};
    }
}
