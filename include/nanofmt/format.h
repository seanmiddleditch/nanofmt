// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "buffer.h"

#include <cstddef>

namespace nanofmt {
    struct string_view;
    struct format_args;
    struct format_spec;

    /// Specialize to implement format support for a type.
    ///
    /// Two member functions must be defined:
    ///
    /// constexpr char const* parse(char const* in, char const* end) noexcept;
    ///
    /// void format(T const& value, buffer& buf);
    template <typename T>
    struct formatter;

    constexpr string_view to_string_view(string_view string) noexcept;
    template <typename StringT>
    constexpr string_view to_string_view(StringT const& string) noexcept;
    template <std::size_t N>
    constexpr string_view to_string_view(char const (&string)[N]) noexcept;
    constexpr string_view to_string_view(char const* zstr) noexcept;

    template <typename FormatT, typename... Args>
    char* format_to(buffer& buf, FormatT&& format_str, Args const&... args);

    template <typename FormatT>
    char* vformat_to(buffer& buf, FormatT&& format_str, format_args&& args);

    template <typename FormatT, typename... Args>
    char* format_to_n(char* dest, std::size_t count, FormatT&& format_str, Args const&... args);

    template <typename FormatT>
    char* vformat_to_n(char* dest, std::size_t count, FormatT&& format_str, format_args&& args);

    template <typename FormatT, std::size_t N, typename... Args>
    char* format_to(char (&dest)[N], FormatT&& format_str, Args const&... args);

    template <typename FormatT, std::size_t N>
    char* vformat_to(char (&dest)[N], FormatT&& format_str, format_args&& args);

    template <typename FormatT, typename... Args>
    std::size_t format_size(FormatT&& format_str, Args const&... args);

    template <typename FormatT>
    std::size_t vformat_size(FormatT&& format_str, format_args&& args);

    template <typename... Args>
    constexpr auto make_format_args(Args const&... args) noexcept;

    constexpr char const* parse_spec(
        char const* in,
        char const* end,
        format_spec& spec,
        char const* allowed_types = nullptr) noexcept;
} // namespace nanofmt

namespace nanofmt::detail {
    char* vformat(buffer& buf, string_view format_str, format_args args);
    constexpr int parse_nonnegative(char const*& start, char const* end) noexcept;
} // namespace nanofmt::detail

/// Default formatter spec options
struct nanofmt::format_spec {
    int width = -1;
    int precision = -1;
    signed char align = -1; // -1 left, 0 center, +1 right
    char sign = '-'; // -, +, or space
    char fill = ' ';
    char type = '\0';
    bool zero_pad = false;
    bool alt_form = false;
    bool locale = false;
};

/// No-op formatter
template <>
struct nanofmt::formatter<void> {
    constexpr char const* parse(char const* in, char const*) noexcept {
        return in;
    }

    template <typename ValueT>
    void format(ValueT const&, buffer&) {}
};

/// Non-owning slice of string data.
struct nanofmt::string_view {
    constexpr string_view() = default;
    constexpr string_view(char const* str, std::size_t len) noexcept : begin(str), end(str + len) {}
    constexpr string_view(char const* str, char const* end) noexcept : begin(str), end(end) {}
    constexpr string_view(char const* zstr) noexcept : begin(zstr), end(zstr + __builtin_strlen(zstr)) {}

    /// Pointer to the data references by the string_view
    constexpr char const* data() const noexcept {
        return begin;
    }

    /// Number of characters referenced by the string view
    constexpr std::size_t size() const noexcept {
        return end - begin;
    }

    char const* begin = nullptr; /// Pointer to the first characte
    char const* end = nullptr;
};

constexpr nanofmt::string_view nanofmt::to_string_view(string_view string) noexcept {
    return string;
}

/// Convert an arbitrary string type to a nanofmt string_view
template <typename StringT>
constexpr nanofmt::string_view nanofmt::to_string_view(StringT const& string) noexcept {
    return {string.data(), string.size()};
}

template <std::size_t N>
constexpr nanofmt::string_view nanofmt::to_string_view(char const (&string)[N]) noexcept {
    // don't assume N is the real length; this is probably a NUL-terminated literal
    return string_view{string};
}

constexpr nanofmt::string_view nanofmt::to_string_view(char const* zstr) noexcept {
    return string_view{zstr};
}

template <typename FormatT>
char* nanofmt::vformat_to(buffer& buf, FormatT&& format_str, format_args&& args) {
    return detail::vformat(buf, to_string_view(format_str), static_cast<format_args&&>(args));
}

template <typename FormatT>
char* nanofmt::vformat_to_n(char* dest, std::size_t count, FormatT&& format_str, format_args&& args) {
    buffer buf(dest, count);
    return detail::vformat(buf, to_string_view(format_str), args);
}

/// Formats a string and arguments into dest, writing no more than count
/// bytes. The output will be NUL-terminated. Returns a pointer to the
/// last character written, which will be the NUL byte itself.
template <typename FormatT, typename... Args>
char* nanofmt::format_to_n(char* dest, std::size_t count, FormatT&& format_str, Args const&... args) {
    buffer buf(dest, count);
    return detail::vformat(buf, to_string_view(format_str), nanofmt::make_format_args(args...));
}

template <typename FormatT, typename... Args>
char* nanofmt::format_to(buffer& buf, FormatT&& format_str, Args const&... args) {
    return detail::vformat(buf, to_string_view(format_str), nanofmt::make_format_args(args...));
}

/// Formats a string and arguments into dest, writing no more than N
/// bytes. The output will be NUL-terminated. Returns a pointer to the
/// last character written, which will be the NUL byte itself.
template <typename FormatT, std::size_t N, typename... Args>
char* nanofmt::format_to(char (&dest)[N], FormatT&& format_str, Args const&... args) {
    buffer buf(dest, N - 1 /*NUL*/);
    char* const end = detail::vformat(buf, to_string_view(format_str), nanofmt::make_format_args(args...));
    *end = '\0';
    return end;
}

/// Returns the number of characters that would be written to a
/// destination buffer (_excluding_ any terminating NUL) for the
/// given format string and arguments
template <typename FormatT, typename... Args>
std::size_t nanofmt::format_size(FormatT&& format_str, Args const&... args) {
    buffer buf(nullptr, 0);
    detail::vformat(buf, to_string_view(format_str), nanofmt::make_format_args(args...));
    return buf.advance;
}

template <typename FormatT>
std::size_t nanofmt::vformat_size(FormatT&& format_str, format_args&& args) {
    buffer buf(nullptr, 0);
    detail::vformat(buf, to_string_view(format_str), args);
    return buf.advance;
}

#include "format_arg.h"
#include "formatter_float.h"
#include "formatter_int.h"
#include "formatter_string.h"

constexpr int nanofmt::detail::parse_nonnegative(char const*& start, char const* end) noexcept {
    if (start == end) {
        return -1;
    }

    if (*start == '0') {
        ++start;
        return 0;
    }

    // there must be at least one non-zero digit
    if (!(*start >= '1' && *start <= '9')) {
        return -1;
    }

    int result = 0;
    while (start != end && *start >= '0' && *start <= '9') {
        result *= 10;
        result += *start - '0';
        ++start;
    }
    return result;
}

/// Parses standard format specification options into the provided
/// spec object. Returns a pointer to the next unconsumed character
/// from the input range.
constexpr char const* nanofmt::parse_spec(
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
    // fixme: fill should be any character except { and } but only when followed
    //  by an alignment
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

    // -- parse locale flag
    //
    if (*in == 'L') {
        spec.locale = true;
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
