// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "config.h"

namespace NANOFMT_NS {
    // clang-format off
    /// @brief Format options for integral values.
    enum class int_format {
        decimal     = 1, ///< Format in base 10
        hex         = 2, ///< Format in base 16
        hex_upper   = 3, ///< Format in base 16, with uppercase letters
        binary      = 4, ///< Format in base 2
    };

    /// Format options for floating-point values.
    enum class float_format {
        /// Format in scientific notation: [-]d.de[+-]dd
        scientific          = 0b0001,
        /// Format in scientific notation, with uppercase letters: [-]d.dE[+-]dd
        scientific_upper    = scientific | 0b1000,
        /// Format in fixed precision: [-]d.dddd
        fixed               = 0b0010,
        /// Format in hexadecimal: [-]1.xxxxp[+-]dd
        hex                 = 0b0100,
        /// Format in hexadecimal: [-]1.XXXXP[+-]dd
        hex_upper           = hex   | 0b1000,
        /// Format in either scientific or fixed precision, depending on the exponent
        general             = fixed | scientific,
        /// Format in either uppercase scientific or fixed precision, depending on the exponent
        general_upper       = fixed | scientific_upper
    };
    // clang-format on

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
    /// @brief Format an integer value to the target buffer.
    /// @param buffer target buffer to write characters to.
    /// @param end the end of the target buffer.
    /// @param value the value to format.
    /// @param fmt formatting options.
    /// @return one past the last character written.
    template <typename IntegerT>
    char* to_chars(char* dest, char const* end, IntegerT value, int_format fmt = int_format::decimal) noexcept;
#else
    char* to_chars(char* dest, char const* end, signed char value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, unsigned char value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, signed short value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, unsigned short value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, signed int value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, unsigned int value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, signed long value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, unsigned long value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(char* dest, char const* end, signed long long value, int_format fmt = int_format::decimal) noexcept;
    char* to_chars(
        char* dest,
        char const* end,
        unsigned long long value,
        int_format fmt = int_format::decimal) noexcept;
#endif

    // plain char is disallowed, cast to signed or unsigned char for integer formatting
    char* to_chars(char* dest, char const* end, char value, int_format) noexcept = delete;

    // bools are disallowed, cast to an integer type if a 0 or 1 format is required
    char* to_chars(char* dest, char const* end, bool value, int_format) noexcept = delete;

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
    /// @brief Format a single-precision floating point value to the target buffer.
    /// @param buffer target buffer to write characters to.
    /// @param end the end of the target buffer.
    /// @param value the value to format.
    /// @param fmt formatting options.
    /// @return one past the last character written.
    template <typename FloatT>
    char* to_chars(char* dest, char const* end, FloatT value, float_format fmt) noexcept;

    /// @brief Format a single-precision floating point value to the target buffer.
    /// @param buffer target buffer to write characters to.
    /// @param end the end of the target buffer.
    /// @param value the value to format.
    /// @param fmt formatting options.
    /// @param precision target precision for the output.
    /// @return one past the last character written.
    template <typename FloatT>
    char* to_chars(char* dest, char const* end, FloatT value, float_format fmt, int precision) noexcept;
#else
    char* to_chars(char* dest, char const* end, float value, float_format fmt) noexcept;
    char* to_chars(char* dest, char const* end, double value, float_format fmt) noexcept;

    char* to_chars(char* dest, char const* end, float value, float_format fmt, int precision) noexcept;
    char* to_chars(char* dest, char const* end, double value, float_format fmt, int precision) noexcept;
#endif
    /// @}

    // to_chars for floating-point types requires explicit use of float_format
    char* to_chars(char* dest, char const* end, float value) noexcept = delete;
    char* to_chars(char* dest, char const* end, double value) noexcept = delete;

    // long doubles are not supported
    char* to_chars(char* dest, char const* end, long double value) noexcept = delete;
    char* to_chars(char* dest, char const* end, long double value, float_format fmt) noexcept = delete;
    char* to_chars(char* dest, char const* end, long double value, float_format fmt, int precision) noexcept = delete;
} // namespace NANOFMT_NS
