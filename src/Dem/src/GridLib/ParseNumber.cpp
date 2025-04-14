//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 23.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "ParseNumber.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

namespace GridLib
{
    namespace
    {
        template <typename IntT>
        IntT from_digit(char c)
        {
            if ('0' <= c && c <= '9')
                return IntT(c - '0');
            auto u = uint8_t(c);
            u &= 0xDFu;
            if ('A' <= u && u <= 'Z')
                return IntT(10 + u - 'A');
            return std::numeric_limits<IntT>::max();
        }

        template <typename T, T Base>
        constexpr T max_preceding_value_negative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) / U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T max_final_digit_negative()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T((U(1) << (sizeof(T) * 8 - 1)) % U(Base));
            }
            else
            {
                return T(0);
            }
        }

        template <typename T, T Base>
        constexpr T max_preceding_value_positive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) / U(Base));
            }
            else
            {
                return T(~T(0)) / Base;
            }
        }

        template <typename T, T Base>
        constexpr T max_final_digit_positive()
        {
            if constexpr (std::is_signed<T>::value)
            {
                using U = typename std::make_unsigned<T>::type;
                return T(U(~(U(1) << (sizeof(T) * 8 - 1))) % U(Base));
            }
            else
            {
                return T(~T(0)) % Base;
            }
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parse_positive_integer_impl(std::string_view str)
        {
            if (str.empty())
                return {};
            IntT value = from_digit<IntT>(str[0]);
            if (value >= Base)
                return {};
            for (size_t i = 1; i < str.size(); ++i)
            {
                auto digit = from_digit<IntT>(str[i]);
                if (digit < Base
                    && (value < max_preceding_value_positive<IntT, Base>()
                        || (value == max_preceding_value_positive<IntT, Base>()
                            && digit <= max_final_digit_positive<IntT, Base>())))
                {
                    value *= Base;
                    value += digit;
                }
                else if (str[i] != '_' || i == str.size() - 1
                         || str[i - 1] == '_')
                {
                    return {};
                }
            }
            return value;
        }

        template <typename IntT, IntT Base>
        std::optional<IntT> parse_negative_integer_impl(std::string_view str)
        {
            if constexpr (std::is_signed<IntT>::value)
            {
                if (str.empty())
                    return {};
                IntT value = from_digit<IntT>(str[0]);
                if (value >= Base)
                    return {};
                for (size_t i = 1; i < str.size(); ++i)
                {
                    auto digit = from_digit<IntT>(str[i]);
                    if (digit < Base
                        && (value < max_preceding_value_negative<IntT, Base>()
                            || (value == max_preceding_value_negative<IntT, Base>()
                                && digit <= max_final_digit_negative<IntT, Base>())))
                    {
                        value *= Base;
                        value += digit;
                    }
                    else if (str[i] != '_' || i == str.size() - 1
                             || str[i - 1] == '_')
                    {
                        return {};
                    }
                }
                return IntT(-value);
            }
            else
            {
                if (str.empty())
                    return {};
                for (char c : str)
                {
                    auto digit = from_digit<IntT>(c);
                    if (digit > 0)
                        return {};
                }
                return IntT(0);
            }
        }

        template <typename IntT>
        std::optional<IntT> parse_integer(std::string_view str, bool detect_base)
        {
            static_assert(std::is_integral<IntT>());

            if (str.empty())
                return {};

            bool positive = true;
            if (str[0] == '-')
            {
                positive = false;
                str = str.substr(1);
            }
            else if (str[0] == '+')
            {
                str = str.substr(1);
            }

            if (str.empty())
                return {};

            if (str[0] == '0' && str.size() >= 3 && detect_base)
            {
                auto str2 = str.substr(2);
                switch (uint8_t(str[1]) | 0x20u)
                {
                case 'b':
                    return positive ? parse_positive_integer_impl<IntT, 2>(str2)
                                    : parse_negative_integer_impl<IntT, 2>(str2);
                case 'o':
                    return positive ? parse_positive_integer_impl<IntT, 8>(str2)
                                    : parse_negative_integer_impl<IntT, 8>(str2);
                case 'x':
                    return positive ? parse_positive_integer_impl<IntT, 16>(str2)
                                    : parse_negative_integer_impl<IntT, 16>(str2);
                default:
                    break;
                }
            }
            if ('0' <= str[0] && str[0] <= '9')
            {
                return positive ? parse_positive_integer_impl<IntT, 10>(str)
                                : parse_negative_integer_impl<IntT, 10>(str);
            }
            if (str == "false" || str == "null")
                return IntT(0);
            if (str == "true")
                return IntT(1);
            return {};
        }

        template <typename T>
        bool parse_impl(std::string_view str, T& value, bool detect_base)
        {
            auto parsed_value = parse_integer<T>(str, detect_base);
            if (parsed_value)
            {
                value = *parsed_value;
                return true;
            }
            return false;
        }
    }

    bool parse(std::string_view str, char& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, int8_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, int16_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, int32_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, int64_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, uint8_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, uint16_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, uint32_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    bool parse(std::string_view str, uint64_t& value, bool detect_base)
    {
        return parse_impl(str, value, detect_base);
    }

    namespace
    {
        int get_digit(char c)
        {
            return int(uint8_t(c) ^ 0x30u);
        }

        template <typename T>
        std::optional<T> parse_floating_point(std::string_view str)
        {
            if (str.empty())
                return {};

            size_t i = 0;
            // Get the sign of the number
            bool negative = false;
            if (str[0] == '-')
            {
                negative = true;
                if (++i == str.size())
                    return {};
            }
            else if (str[0] == '+')
            {
                if (++i == str.size())
                    return {};
            }

            // Get the integer value
            auto value = T(get_digit(str[i]));
            if (value > 9)
            {
                if (str == "Infinity" || str == "null" || str == "+Infinity")
                    return std::numeric_limits<T>::infinity();
                if (str == "-Infinity")
                    return -std::numeric_limits<T>::infinity();
                if (str == "NaN")
                    return std::numeric_limits<T>::quiet_NaN();
                return {};
            }

            bool underscore = false;
            for (++i; i < str.size(); ++i)
            {
                auto digit = get_digit(str[i]);
                if (digit <= 9)
                {
                    value *= 10;
                    value += digit;
                    underscore = false;
                }
                else if (str[i] != '_' || underscore)
                {
                    break;
                }
                else
                {
                    underscore = true;
                }
            }

            if (underscore)
                return {};

            if (i == str.size())
                return !negative ? value : -value;

            // Get the fraction
            underscore = true; // Makes underscore after point illegal.
            int decimals = 0;
            T fraction = {};
            if (str[i] == '.')
            {
                for (++i; i < str.size(); ++i)
                {
                    auto digit = get_digit(str[i]);
                    if (digit <= 9)
                    {
                        fraction *= 10;
                        fraction += digit;
                        underscore = false;
                        ++decimals;
                    }
                    else if (str[i] != '_' || underscore)
                    {
                        break;
                    }
                    else
                    {
                        underscore = true;
                    }
                }
            }

            // Get the exponent
            int exponent = 0;
            if (i != str.size())
            {
                // Accept both e/E and d/D (FORTRAN) as exponent character.
                if ((uint8_t(str[i]) & 0xDEu) != 'D')
                    return {};

                if (++i == str.size())
                    return {};

                bool negative_exponent = false;
                if (str[i] == '-')
                {
                    negative_exponent = true;
                    if (++i == str.size())
                        return {};
                }
                else if (str[i] == '+')
                {
                    if (++i == str.size())
                        return {};
                }

                exponent += get_digit(str[i]);
                if (exponent > 9)
                    return {};

                for (++i; i != str.size(); ++i)
                {
                    auto digit = get_digit(str[i]);
                    if (digit <= 9)
                    {
                        exponent *= 10;
                        exponent += digit;
                        underscore = false;
                    }
                    else if (str[i] != '_' || underscore)
                    {
                        return {};
                    }
                    else
                    {
                        underscore = true;
                    }
                    if (exponent > std::numeric_limits<T>::max_exponent10)
                        return {};
                }
                if (negative_exponent)
                    exponent = -exponent;
            }

            if (exponent)
                value *= pow(T(10), exponent);
            if (fraction != 0)
                value += fraction * pow(T(10), exponent - decimals);

            // Add the sign
            if (negative)
                value = -value;

            return value;
        }

        template <typename T>
        bool parse_impl(std::string_view str, T& value)
        {
            if (auto parsed_value = parse_floating_point<T>(str))
            {
                value = *parsed_value;
                return true;
            }
            return false;
        }
    }

    bool parse(std::string_view str, float& value)
    {
        return parse_impl(str, value);
    }

    bool parse(std::string_view str, double& value)
    {
        return parse_impl(str, value);
    }

    bool parse(std::string_view str, long double& value)
    {
        return parse_impl(str, value);
    }
}
