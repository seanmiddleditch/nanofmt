// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

namespace nanofmt {
    // clang-format off
    enum class int_format {
        decimal     = 1,
        hex         = 2,
        hex_upper   = 3,
        binary      = 4
    };
    
    enum class float_format {
        scientific          = 0b0001,
        scientific_upper    = scientific | 0b1000,
        fixed               = 0b0010,
        hex                 = 0b0100,
        hex_upper           = hex   | 0b1000,
        general             = fixed | scientific,
        general_upper       = fixed | scientific_upper
    };
    // clang-format on

    // supported integral types - defaults to decimal formatting
    char* to_chars(char* buffer, char const* end, signed char value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* buffer, char const* end, unsigned char value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* buffer, char const* end, signed short value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* buffer, char const* end, unsigned short value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* buffer, char const* end, signed int value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* buffer, char const* end, unsigned int value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* buffer, char const* end, signed long value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* buffer, char const* end, unsigned long value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(
        char* buffer,
        char const* end,
        signed long long value,
        int_format fmt = int_format::decimal) noexcept;
    char* to_chars(
        char* buffer,
        char const* end,
        unsigned long long value,
        int_format fmt = int_format::decimal) noexcept;

    // plain char is disallowed, cast to signed or unsigned char for integer formatting
    char* to_chars(char* buffer, char const* end, char value, int_format) noexcept = delete;

    // bools are disallowed, cast to an integer type if a 0 or 1 format is required
    char* to_chars(char* buffer, char const* end, bool value, int_format) noexcept = delete;

    // supported floating-point types; note that float_format is required
    char* to_chars(char* buffer, char const* end, float value, float_format fmt) noexcept;
    char* to_chars(char* buffer, char const* end, double value, float_format fmt) noexcept;

    char* to_chars(char* buffer, char const* end, float value, float_format fmt, int precision) noexcept;
    char* to_chars(char* buffer, char const* end, double value, float_format fmt, int precision) noexcept;

    // to_chars for floating-point types requires explicit use of float_format
    char* to_chars(char* buffer, char const* end, float value) noexcept = delete;
    char* to_chars(char* buffer, char const* end, double value) noexcept = delete;

    // long doubles are not supported
    char* to_chars(char* buffer, char const* end, long double value) noexcept = delete;
    char* to_chars(char* buffer, char const* end, long double value, float_format fmt) noexcept = delete;
    char* to_chars(char* buffer, char const* end, long double value, float_format fmt, int precision) noexcept = delete;
} // namespace nanofmt
