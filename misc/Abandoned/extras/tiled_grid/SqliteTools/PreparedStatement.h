#pragma once

#include <cstdint>
#include <string>
#include <sqlite3.h>
#include <variant>

namespace SqliteTools
{
    enum class Lifetime
    {
        STATIC,
        TRANSIENT
    };

    using LifetimeManagement = std::variant<void (*)(void*), Lifetime>;

    class PreparedStatement
    {
    public:
        PreparedStatement(sqlite3* db,
                          std::string_view statement);

        ~PreparedStatement();

        PreparedStatement(const PreparedStatement&) = delete;

        PreparedStatement(PreparedStatement&&) noexcept;

        PreparedStatement& operator=(const PreparedStatement&) = delete;

        PreparedStatement& operator=(PreparedStatement&&) noexcept;

        bool step();

        void reset();

        void finalize();

        void bind(int index, int value);

        void bind(int index, double value);

        void bind(int index, int64_t value);

        void bind(int index, std::string_view value,
                  LifetimeManagement lifetime = Lifetime::TRANSIENT);

        void bind(int index, const void* blob, size_t size,
                  LifetimeManagement lifetime = Lifetime::TRANSIENT);

        [[nodiscard]] int column_type(int index) const;

        [[nodiscard]] int column_count() const;

        [[nodiscard]] double double_column(int index) const;

        [[nodiscard]] int int_column(int index) const;

        [[nodiscard]] int64_t int64_column(int index) const;

        [[nodiscard]] std::string_view string_column(int index) const;

        [[nodiscard]] std::pair<const void*, size_t> blob_column(int index) const;

    private:
        sqlite3_stmt* statement_;
    };
}
