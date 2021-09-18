// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "nanofmt/format.h"
#include "nanofmt/to_chars.h"

#include <cmath>

// utilities
namespace NANOFMT_NS::detail {
    static constexpr void format_int_chars(
        buffer& buf,
        char const* digits,
        size_t count,
        bool negative,
        format_spec const& spec) noexcept;
    static constexpr int_format select_int_format(char type) noexcept;
    static constexpr char const* parse_int_spec(char const* in, char const* end, format_spec& spec) noexcept;
    static constexpr char const* parse_float_spec(char const* in, char const* end, format_spec& spec) noexcept;
    static constexpr std::size_t strnlen(char const* string, std::size_t max_length) noexcept;
    static constexpr void format_char(char value, buffer& buf, format_spec const& spec) noexcept;
    template <typename IntT>
    static constexpr void format_int(IntT value, buffer& buf, format_spec const& spec) noexcept;
    static constexpr void format_string(
        char const* value,
        std::size_t length,
        buffer& buf,
        format_spec const& spec) noexcept;
    template <typename FloatT>
    static constexpr void format_float(FloatT value, buffer& buf, format_spec const& spec) noexcept;

    static constexpr void format_int_chars(
        buffer& buf,
        char const* digits,
        size_t count,
        bool negative,
        format_spec const& spec) noexcept {
        if (*digits == '-') {
            ++digits;
            --count;
        }

        char const sign_char = negative ? '-' : (spec.sign == '+') ? '+' : (spec.sign == ' ') ? ' ' : '\0';

        size_t const zero_padding =
            spec.zero_pad && (spec.width > count + (sign_char != '\0')) ? spec.width - count - (sign_char != '\0') : 0;

        size_t const total_length = (sign_char != '\0') + count + zero_padding;

        if (spec.width > 0 && spec.align > 0 && spec.width > total_length) {
            buf.fill_n(spec.fill, spec.width - total_length);
        }

        if (sign_char != '\0') {
            buf.append(sign_char);
        }
        buf.fill_n('0', zero_padding);
        buf.append(digits, count);
    }

    static constexpr int_format select_int_format(char type) noexcept {
        switch (type) {
            default:
            case 'd':
                return int_format::decimal;
            case 'x':
                return int_format::hex;
            case 'X':
                return int_format::hex_upper;
            case 'b':
                return int_format::binary;
        }
    }

    static constexpr char const* parse_int_spec(char const* in, char const* end, format_spec& spec) noexcept {
        spec.align = +1; /* right-align by default */
        return parse_spec(in, end, spec, "bBcdoxX");
    }

    static constexpr char const* parse_float_spec(char const* in, char const* end, format_spec& spec) noexcept {
        return parse_spec(in, end, spec, "aAeEfFgG");
    }

    static constexpr std::size_t strnlen(char const* string, std::size_t max_length) noexcept {
        for (std::size_t length = 0; length != max_length; ++length) {
            if (string[length] == '\0') {
                return length;
            }
        }
        return max_length;
    }

    static constexpr void format_char(char value, buffer& buf, format_spec const& spec) noexcept {
        switch (spec.type) {
            case '\0':
            case 'c':
                buf.append(value);
                break;
            default:
                return format_int(static_cast<int>(value), buf, spec);
        }
    }

    template <typename IntT>
    static constexpr void format_int(IntT value, buffer& buf, format_spec const& spec) noexcept {
        if (spec.type == 'c') {
            return format_char(static_cast<char>(value), buf, spec);
        }

        // binary encoding is the widest; FIXME: this is icky
        char buffer[sizeof(value) * 8];

        size_t const available = buf.end - buf.pos;
        size_t const length = spec.precision < available ? spec.precision : available;

        const char* const end = to_chars(buffer, buffer + length, value, select_int_format(spec.type));
        format_int_chars(buf, buffer, end - buffer, value < 0, spec);
    }

    static constexpr void format_string(
        char const* value,
        std::size_t length,
        buffer& buf,
        format_spec const& spec) noexcept {
        if (spec.width < 0 || length >= static_cast<size_t>(spec.width)) {
            buf.append(value, length);
            return;
        }

        auto const padding = static_cast<size_t>(spec.width) - length;
        if (spec.align < 0) {
            buf.append(value, length);
            buf.fill_n(' ', padding);
        }
        else if (spec.align > 0) {
            buf.fill_n(' ', padding);
            buf.append(value, length);
        }
        else {
            auto const front_padding = padding / 2;
            auto const back_padding = padding - front_padding;
            buf.fill_n(' ', front_padding);
            buf.append(value, length);
            buf.fill_n(' ', back_padding);
        }
    }

    template <typename FloatT>
    static constexpr void format_float(FloatT value, buffer& buf, format_spec const& spec) noexcept {
        if (!std::signbit(value)) {
            if (spec.sign == '+') {
                buf.append('+');
            }
            else if (spec.sign == ' ') {
                buf.append(' ');
            }
        }

        switch (spec.type) {
            default:
            case 'g':
            case 'G':
                buf.advance_to(to_chars(
                    buf.pos,
                    buf.end,
                    value,
                    spec.type == 'G' ? float_format::general_upper : float_format::general,
                    spec.precision));
                break;
            case 'e':
            case 'E':
                buf.advance_to(to_chars(
                    buf.pos,
                    buf.end,
                    value,
                    spec.type == 'E' ? float_format::scientific_upper : float_format::scientific,
                    spec.precision < 0 ? 6 : spec.precision));
                break;
            case 'f':
            case 'F':
                buf.advance_to(
                    to_chars(buf.pos, buf.end, value, float_format::fixed, spec.precision < 0 ? 6 : spec.precision));
                break;
        }
    }
} // namespace NANOFMT_NS::detail

namespace NANOFMT_NS::detail {
    template <>
    char const* default_formatter<char>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void default_formatter<char>::format(char value, buffer& buf) noexcept {
        format_char(value, buf, spec);
    }

    template <>
    char const* default_formatter<signed int>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void default_formatter<signed int>::format(signed int value, buffer& buf) noexcept {
        return format_int(value, buf, spec);
    }

    template <>
    char const* default_formatter<unsigned int>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void default_formatter<unsigned int>::format(unsigned int value, buffer& buf) noexcept {
        return format_int(value, buf, spec);
    }

    template <>
    char const* default_formatter<signed long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void default_formatter<signed long>::format(signed long value, buffer& buf) noexcept {
        return format_int(value, buf, spec);
    }

    template <>
    char const* default_formatter<unsigned long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void default_formatter<unsigned long>::format(unsigned long value, buffer& buf) noexcept {
        return format_int(value, buf, spec);
    }

    template <>
    char const* default_formatter<signed long long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void default_formatter<signed long long>::format(signed long long value, buffer& buf) noexcept {
        return format_int(value, buf, spec);
    }

    template <>
    char const* default_formatter<unsigned long long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void default_formatter<unsigned long long>::format(unsigned long long value, buffer& buf) noexcept {
        return format_int(value, buf, spec);
    }

    template <>
    char const* default_formatter<float>::parse(char const* in, char const* end) noexcept {
        return parse_float_spec(in, end, spec);
    }

    template <>
    void default_formatter<float>::format(float value, buffer& buf) noexcept {
        return format_float(value, buf, spec);
    }

    template <>
    char const* default_formatter<double>::parse(char const* in, char const* end) noexcept {
        return parse_float_spec(in, end, spec);
    }

    template <>
    void default_formatter<double>::format(double value, buffer& buf) noexcept {
        return format_float(value, buf, spec);
    }

    template <>
    char const* default_formatter<bool>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "sbBcdoxX");
    }

    template <>
    void default_formatter<bool>::format(bool value, buffer& buf) noexcept {
        switch (spec.type) {
            case '\0':
            case 's':
                buf.append(value ? "true" : "false");
                break;
            default:
                format_int(static_cast<unsigned char>(value), buf, spec);
                return;
        }
    }

    template <>
    char const* default_formatter<void const*>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "p");
    }

    template <>
    void default_formatter<void const*>::format(void const* value, buffer& buf) noexcept {
        // hex encoding is 2 chars per octet
        char buffer[sizeof(value) * 2];
        char const* const end =
            to_chars(buffer, buffer + sizeof buffer, reinterpret_cast<std::uintptr_t>(value), int_format::hex);
        buf.append("0x");
        buf.append(buffer, end - buffer);
    }

    template <>
    char const* default_formatter<char const*>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "s");
    }

    template <>
    void default_formatter<char const*>::format(char const* value, buffer& buf) noexcept {
        if (value != nullptr) {
            format_string(value, __builtin_strlen(value), buf, spec);
        }
    }

    char const* char_buf_formatter::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "s");
    }

    void char_buf_formatter::format(char const* value, buffer& buf) noexcept {
        if (value != nullptr) {
            format_string(value, strnlen(value, max_length), buf, spec);
        }
    }

    char const* default_formatter<format_string_view>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "s");
    }

    void default_formatter<format_string_view>::format(format_string_view value, buffer& buf) noexcept {
        format_string(value.string, value.length, buf, spec);
    }
} // namespace NANOFMT_NS::detail
