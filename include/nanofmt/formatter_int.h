// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "to_chars.h"

#include <cmath>
#include <cstdint>

namespace nanofmt::detail {
    struct int_formatter_base {
        format_spec spec;

        constexpr char const* parse(char const* in, char const* end) noexcept {
            spec.align = +1; // right-align by default
            return parse_spec(in, end, spec, "bBcdoxX");
        }

        void do_format(buffer& buf, char const* digits, size_t count, bool negative) const noexcept {
            if (*digits == '-') {
                ++digits;
                --count;
            }

            char const sign_char = negative ? '-' : (spec.sign == '+') ? '+' : (spec.sign == ' ') ? ' ' : '\0';

            size_t const zero_padding = spec.zero_pad && (spec.width > count + (sign_char != '\0'))
                ? spec.width - count - (sign_char != '\0')
                : 0;

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

        static int_format select_format(char type) noexcept {
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
    };

    template <typename IntT>
    struct int_formatter : int_formatter_base {
        void format(IntT value, buffer& buf) const noexcept {
            if (spec.type == 'c') {
                buf.append(static_cast<char>(value));
                return;
            }

            // binary encoding is the widest; FIXME: this is icky
            char buffer[sizeof(value) * 8];

            size_t const available = buf.end - buf.pos;
            size_t const length = spec.precision < available ? spec.precision : available;

            const char* const end = to_chars(buffer, buffer + length, value, select_format(spec.type));
            do_format(buf, buffer, end - buffer, value < 0);
        }
    };

    struct pointer_formatter : int_formatter_base {
        format_spec spec;

        constexpr char const* parse(char const* in, char const* end) noexcept {
            spec.align = +1; // right-align by default
            return parse_spec(in, end, spec, "xX");
        }

        void format(void const* const value, buffer& buf) const noexcept {
            auto const ptr_value = reinterpret_cast<std::uintptr_t>(value);

            // buffer for hex encoding, two characters per octet
            char buffer[sizeof(ptr_value) * 2];
            char* pos = buffer;

            switch (spec.type) {
                default:
                case 'x':
                    pos = to_chars(buffer, buffer + sizeof buffer, ptr_value, int_format::hex);
                    break;
                case 'X':
                    pos = to_chars(buffer, buffer + sizeof buffer, ptr_value, int_format::hex_upper);
                    break;
            }

            // prefer, zero-padding, digits
            buf.append(spec.type == 'X' ? "0X" : "0x");
            buf.fill_n('0', sizeof buffer - (pos - buffer));
            buf.append(buffer, pos - buffer);
        }
    };
} // namespace nanofmt::detail

namespace nanofmt {
    template <>
    struct formatter<signed char> : detail::int_formatter<int> {};
    template <>
    struct formatter<unsigned char> : detail::int_formatter<unsigned> {};

    template <>
    struct formatter<signed short> : detail::int_formatter<int> {};
    template <>
    struct formatter<unsigned short> : detail::int_formatter<unsigned> {};

    template <>
    struct formatter<signed int> : detail::int_formatter<int> {};
    template <>
    struct formatter<unsigned int> : detail::int_formatter<unsigned> {};

    // note: this slightly pessimizes long on platforms where sizeof(long)==sizeof(int)
    template <>
    struct formatter<signed long> : detail::int_formatter<int long long> {};
    template <>
    struct formatter<unsigned long> : detail::int_formatter<unsigned long long> {};

    template <>
    struct formatter<signed long long> : detail::int_formatter<long long> {};
    template <>
    struct formatter<unsigned long long> : detail::int_formatter<unsigned long long> {};

    // note: bool is weird, it's not really int-like; FIXME: move?
    template <>
    struct formatter<bool> : detail::int_formatter<int> {
        void format(bool value, buffer& buf) const noexcept {
            if (spec.type == '\0') {
                buf.append(value ? "true" : "false");
                return;
            }

            return int_formatter<int>::format(static_cast<int>(value), buf);
        }
    };

    // note: pointers format at integers (but always with the 0x prefix)
    // FIXME: these won't catch user-defined pointers
    template <>
    struct formatter<decltype(nullptr)> : detail::pointer_formatter {};
    template <>
    struct formatter<void*> : detail::pointer_formatter {};
    template <>
    struct formatter<void const*> : detail::pointer_formatter {};
} // namespace nanofmt
