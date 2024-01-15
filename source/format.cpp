// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "parse_utils.h"
#include "nanofmt/charconv.h"
#include "nanofmt/format.h"

#include <cmath>
#include <cstdint>

namespace NANOFMT_NS {
    namespace detail {
        static constexpr char const* parse_spec(
            char const* in,
            char const* end,
            format_spec& spec,
            char const* allowed_types) noexcept;
        static void format_int_chars(
            format_output& out,
            char const* digits,
            size_t count,
            bool negative,
            format_spec const& spec) noexcept;
        static constexpr int_format select_int_format(char type) noexcept;
        static constexpr char const* parse_int_spec(char const* in, char const* end, format_spec& spec) noexcept;
#if NANOFMT_FLOAT
        static constexpr char const* parse_float_spec(char const* in, char const* end, format_spec& spec) noexcept;
#endif
        static constexpr std::size_t strnlen(char const* string, std::size_t max_length) noexcept;
        static void format_char_impl(char value, format_output& out, format_spec const& spec) noexcept;
        template <typename IntT>
        static void format_int_impl(IntT value, format_output& out, format_spec const& spec) noexcept;
        static void format_string_impl(
            char const* value,
            std::size_t length,
            format_output& out,
            format_spec const& spec) noexcept;
        template <typename FloatT>
        static void format_float_impl(FloatT value, format_output& out, format_spec const& spec) noexcept;
    } // namespace detail

    template <>
    char const* detail::default_formatter<char>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<char>::format(char value, format_output& out) noexcept {
        format_char_impl(value, out, spec);
    }

    template <>
    char const* detail::default_formatter<signed int>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<signed int>::format(signed int value, format_output& out) noexcept {
        return format_int_impl(value, out, spec);
    }

    template <>
    char const* detail::default_formatter<unsigned int>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<unsigned int>::format(unsigned int value, format_output& out) noexcept {
        return format_int_impl(value, out, spec);
    }

    template <>
    char const* detail::default_formatter<signed long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<signed long>::format(signed long value, format_output& out) noexcept {
        return format_int_impl(value, out, spec);
    }

    template <>
    char const* detail::default_formatter<unsigned long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<unsigned long>::format(unsigned long value, format_output& out) noexcept {
        return format_int_impl(value, out, spec);
    }

    template <>
    char const* detail::default_formatter<signed long long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<signed long long>::format(signed long long value, format_output& out) noexcept {
        return format_int_impl(value, out, spec);
    }

    template <>
    char const* detail::default_formatter<unsigned long long>::parse(char const* in, char const* end) noexcept {
        return parse_int_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<unsigned long long>::format(unsigned long long value, format_output& out) noexcept {
        return format_int_impl(value, out, spec);
    }

#if NANOFMT_FLOAT
    template <>
    char const* detail::default_formatter<float>::parse(char const* in, char const* end) noexcept {
        return parse_float_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<float>::format(float value, format_output& out) noexcept {
        return format_float_impl(value, out, spec);
    }

    template <>
    char const* detail::default_formatter<double>::parse(char const* in, char const* end) noexcept {
        return parse_float_spec(in, end, spec);
    }

    template <>
    void detail::default_formatter<double>::format(double value, format_output& out) noexcept {
        return format_float_impl(value, out, spec);
    }
#endif

    template <>
    char const* detail::default_formatter<bool>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "sbBcdoxX");
    }

    template <>
    void detail::default_formatter<bool>::format(bool value, format_output& out) noexcept {
        switch (spec.type) {
            case '\0':
            case 's':
                out.append(value ? "true" : "false");
                break;
            default:
                format_int_impl(static_cast<unsigned char>(value), out, spec);
                return;
        }
    }

    template <>
    char const* detail::default_formatter<void const*>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "p");
    }

    template <>
    void detail::default_formatter<void const*>::format(void const* value, format_output& out) noexcept {
        // hex encoding is 2 chars per octet
        char chars[sizeof(value) * 2];
        char const* const end =
            to_chars(chars, chars + sizeof chars, reinterpret_cast<std::uintptr_t>(value), int_format::hex);
        out.append("0x");
        out.append(chars, end - chars);
    }

    template <>
    char const* detail::default_formatter<char const*>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "s");
    }

    template <>
    void detail::default_formatter<char const*>::format(char const* value, format_output& out) noexcept {
        if (value != nullptr) {
            format_string_impl(value, __builtin_strlen(value), out, spec);
        }
    }

    template <>
    char const* detail::default_formatter<detail::char_buffer>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "s");
    }

    template <>
    void detail::default_formatter<detail::char_buffer>::format(char_buffer value, format_output& out) noexcept {
        format_string_impl(value.chars, strnlen(value.chars, value.max_length), out, spec);
    }

    template <>
    char const* detail::default_formatter<format_string_view>::parse(char const* in, char const* end) noexcept {
        return parse_spec(in, end, spec, "s");
    }

    template <>
    void detail::default_formatter<format_string_view>::format(format_string_view value, format_output& out) noexcept {
        format_string_impl(value.string, value.length, out, spec);
    }

    format_output detail::vformat(format_output out, format_string format_str, format_args args) {
        int arg_next_index = 0;
        bool arg_auto_index = true;

        char const* input = format_str.begin;
        char const* input_begin = input;
        char const* const input_end = format_str.end;

        while (input != input_end) {
            if (*input != '{') {
                ++input;
                continue;
            }

            // write out the string so far, since we don't write characters immediately
            out.append(input_begin, input - input_begin);

            ++input; // swallow the {

            // if we hit the end of the input, we have an incomplete format, and nothing else we can do
            if (input == input_end) {
                return out;
            }

            // if we just have another { then take it as a literal character by starting our next begin here,
            // so it'll get written next time we write out the begin; nothing else to do for formatting here
            if (*input == '{') {
                input_begin = input++;
                continue;
            }

            // determine argument
            int arg_index = 0;
            if (arg_index = detail::parse_nonnegative(input, input_end); arg_index != -1) {
                arg_auto_index = false;
            }
            else if (arg_auto_index) {
                arg_index = arg_next_index++;
            }
            else {
                // we received a non-explicit index after an explicit index
                return out;
            }

            // extract formatter specification/arguments
            char const** spec = nullptr;
            if (input != input_end && *input == ':') {
                spec = &++input;
            }

            // format the value
            args.format(arg_index, spec, input_end, out);
            if (spec != nullptr) {
                input = *spec;
            }

            // consume parse specification, and any trailing }
            if (input != input_end && *input == '}') {
                ++input;
            }

            // mark where the next text run will begin
            input_begin = input;
        }

        // write out tail end of format string
        return out.append(input_begin, input - input_begin);
    }

    void format_args::format(unsigned index, char const** in, char const* end, format_output& out) const {
        using types = format_arg::type;

        if (index >= count) {
            return;
        }

        format_arg const& value = values[index];

        auto invoke = [in, end, &out](auto value) {
            formatter<decltype(value)> fmt;
            if (in != nullptr) {
                *in = fmt.parse(*in, end);
            }
            fmt.format(value, out);
        };

        switch (value.tag) {
            case types::t_mono:
                return;
            case types::t_char:
                return invoke(value.v_char);
            case types::t_int:
                return invoke(value.v_int);
            case types::t_uint:
                return invoke(value.v_unsigned);
            case types::t_long:
                return invoke(value.v_long);
            case types::t_ulong:
                return invoke(value.v_ulong);
            case types::t_longlong:
                return invoke(value.v_longlong);
            case types::t_ulonglong:
                return invoke(value.v_ulonglong);
#if NANOFMT_FLOAT
            case types::t_float:
                return invoke(value.v_float);
            case types::t_double:
                return invoke(value.v_double);
#endif
            case types::t_bool:
                return invoke(value.v_bool);
            case types::t_cstring:
                return invoke(value.v_cstring);
            case types::t_voidptr:
                return invoke(value.v_voidptr);
            case types::t_custom:
                return value.v_custom.thunk(value.v_custom.value, in, end, out);
            default:
                break;
        }
    }

    static constexpr char const* detail::parse_spec(
        char const* in,
        char const* end,
        format_spec& spec,
        char const* allowed_types) noexcept {
        if (in == end) {
            return in;
        }

        auto const is_align = [](char const* c, char const* e) noexcept {
            return c != e && (*c == '<' || *c == '>' || *c == '^');
        };

        // -- parse fill -
        //
        if (*in != '{' && *in != '}' && is_align(in + 1, end)) {
            spec.fill = *in;
            ++in;

            if (in == end) {
                return in;
            }
        }

        // -- parse alignment --
        //
        switch (*in) {
            case '<':
                spec.align = -1;
                ++in;
                break;
            case '>':
                spec.align = +1;
                ++in;
                break;
            case '^':
                spec.align = 0;
                ++in;
                break;
            default:
                break;
        }
        if (in == end) {
            return in;
        }

        // -- parse sign --
        //
        switch (*in) {
            case '+':
            case '-':
            case ' ':
                spec.sign = *in++;
                if (in == end) {
                    return in;
                }
                break;
            default:
                break;
        }

        // -- parse alternate form flag --
        //
        if (*in == '#') {
            spec.alt_form = true;
            ++in;
            if (in == end) {
                return in;
            }
        }

        // -- parse zero pad flag --
        //
        if (*in == '0') {
            spec.zero_pad = true;
            ++in;
            if (in == end) {
                return in;
            }
        }

        // -- parse width
        //
        if (int const width = detail::parse_nonnegative(in, end); width >= 0) {
            spec.width = width;
            if (in == end) {
                return in;
            }

            // a width of 0 is not allowed
            if (width == 0) {
                return --in;
            }
        }

        // -- parse precision
        //
        if (*in == '.') {
            ++in;
            int const precision = detail::parse_nonnegative(in, end);

            if (precision < 0 || in == end) {
                return in;
            }

            spec.precision = precision;
        }

        // -- parse locale flag (ignored)
        //
        if (*in == 'L') {
            ++in;
            if (in == end) {
                return in;
            }
        }

        // -- parse type
        //
        if (allowed_types != nullptr) {
            for (char const* t = allowed_types; *t != 0; ++t) {
                if (*in == *t) {
                    spec.type = *in++;
                    break;
                }
            }
        }

        return in;
    }

    void detail::format_int_chars(
        format_output& out,
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
            spec.zero_pad && ((spec.width > 0 && static_cast<std::size_t>(spec.width) > count + (sign_char != '\0')))
            ? spec.width - count - (sign_char != '\0')
            : 0;

        size_t const total_length = (sign_char != '\0') + count + zero_padding;

        if (spec.width > 0 && spec.align > 0 && static_cast<std::size_t>(spec.width) > total_length) {
            out.fill_n(spec.fill, static_cast<std::size_t>(spec.width) - total_length);
        }

        if (sign_char != '\0') {
            out.put(sign_char);
        }
        out.fill_n('0', zero_padding);
        out.append(digits, count);
    }

    constexpr int_format detail::select_int_format(char type) noexcept {
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

    constexpr char const* detail::parse_int_spec(char const* in, char const* end, format_spec& spec) noexcept {
        spec.align = +1; /* right-align by default */
        return parse_spec(in, end, spec, "bBcdoxX");
    }

#if NANOFMT_FLOAT
    constexpr char const* detail::parse_float_spec(char const* in, char const* end, format_spec& spec) noexcept {
        return parse_spec(in, end, spec, "aAeEfFgG");
    }
#endif

    constexpr std::size_t detail::strnlen(char const* string, std::size_t max_length) noexcept {
        for (std::size_t length = 0; length != max_length; ++length) {
            if (string[length] == '\0') {
                return length;
            }
        }
        return max_length;
    }

    void detail::format_char_impl(char value, format_output& out, format_spec const& spec) noexcept {
        switch (spec.type) {
            case '\0':
            case 'c':
                out.put(value);
                break;
            default:
                return format_int_impl(static_cast<int>(value), out, spec);
        }
    }

    template <typename IntT>
    void detail::format_int_impl(IntT value, format_output& out, format_spec const& spec) noexcept {
        if (spec.type == 'c') {
            return format_char_impl(static_cast<char>(value), out, spec);
        }

        // binary encoding is the widest; FIXME: this is icky
        char chars[sizeof(value) * 8] = {
            0,
        };

        size_t const length = (spec.precision >= 0 && static_cast<std::size_t>(spec.precision) < sizeof(chars))
            ? spec.precision
            : sizeof(chars);

        const char* const end = to_chars(chars, chars + length, value, select_int_format(spec.type));
        format_int_chars(out, chars, end - chars, value < 0, spec);
    }

    void detail::format_string_impl(
        char const* value,
        std::size_t length,
        format_output& out,
        format_spec const& spec) noexcept {
        if (spec.width < 0 || length >= static_cast<size_t>(spec.width)) {
            out.append(value, length);
            return;
        }

        auto const padding = static_cast<size_t>(spec.width) - length;
        if (spec.align < 0) {
            out.append(value, length);
            out.fill_n(' ', padding);
        }
        else if (spec.align > 0) {
            out.fill_n(' ', padding);
            out.append(value, length);
        }
        else {
            auto const front_padding = padding / 2;
            auto const back_padding = padding - front_padding;
            out.fill_n(' ', front_padding);
            out.append(value, length);
            out.fill_n(' ', back_padding);
        }
    }

    template <typename FloatT>
    void detail::format_float_impl(FloatT value, format_output& out, format_spec const& spec) noexcept {
        if (!std::signbit(value)) {
            if (spec.sign == '+') {
                out.put('+');
            }
            else if (spec.sign == ' ') {
                out.put(' ');
            }
        }

        switch (spec.type) {
            default:
            case 'g':
            case 'G':
                out.advance_to(to_chars(
                    out.out(),
                    out.end,
                    value,
                    spec.type == 'G' ? float_format::general_upper : float_format::general,
                    spec.precision));
                break;
            case 'e':
            case 'E':
                out.advance_to(to_chars(
                    out.out(),
                    out.end,
                    value,
                    spec.type == 'E' ? float_format::scientific_upper : float_format::scientific,
                    spec.precision < 0 ? 6 : spec.precision));
                break;
            case 'f':
            case 'F':
                out.advance_to(
                    to_chars(out.out(), out.end, value, float_format::fixed, spec.precision < 0 ? 6 : spec.precision));
                break;
        }
    }
} // namespace NANOFMT_NS
