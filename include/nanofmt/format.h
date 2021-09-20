// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#ifndef NANOFMT_FORMAT_H_
#define NANOFMT_FORMAT_H_ 1
#pragma once

#include "config.h"

#include <cstddef>
#include <type_traits>

namespace NANOFMT_NS {
    /// Type-erased wrapper for a formattable value.
    struct format_arg;

    /// List of format args.
    ///
    /// Only use this type as a temporary value!
    struct format_args;

    /// Wrapper for format strings.
    struct format_string {
        constexpr format_string() noexcept = default;
        constexpr format_string(char const* string, std::size_t length) noexcept
            : begin(string)
            , end(string + length) {}
        template <std::size_t N>
        constexpr /*implicit*/ format_string(char const (&str)[N]) noexcept
            : begin(str)
            , end(begin + __builtin_strlen(begin)) {}
        constexpr explicit format_string(char const* const zstr) noexcept
            : begin(zstr)
            , end(begin + __builtin_strlen(begin)) {}

        template <typename StringT>
        constexpr format_string(StringT const& string) noexcept;

        char const* begin = nullptr;
        char const* end = nullptr;
    };

    /// Small wrapper to assist in formatting types like std::string_view.
    struct format_string_view;

    /// Wrapper around a destination sequence of characters.
    ///
    /// Counts the number of characters that are written to the buffer,
    /// excluding truncating, and stores them in the advance member.
    ///
    /// Use advance_to to update the pos pointer to ensure the advance field
    /// is updated appropriately.
    struct format_output {
        char* pos = nullptr;
        char const* end = nullptr;
        std::size_t advance = 0;

        constexpr format_output& append(char const* zstr) noexcept;
        constexpr format_output& append(char const* source, std::size_t length) noexcept;
        constexpr format_output& append(char ch) noexcept;

        constexpr format_output& fill_n(char ch, std::size_t count) noexcept;

        template <typename... Args>
        format_output& format(format_string fmt, Args const&... args);

        inline format_output& vformat(format_string fmt, format_args&& args);

        template <typename ValueT>
        format_output& format_value(ValueT const& value, format_string spec = {});

        constexpr format_output& advance_to(char* p) noexcept;
    };

    /// Holds a list of N format_value objects.
    ///
    /// This is primarily meant to be an intermediate that holds onto values
    /// as a temporary object, and will usually be converted to format_args.
    template <std::size_t N>
    struct format_arg_store;

    /// Specialize to implement format support for a type.
    ///
    /// Two member functions must be defined:
    ///
    /// constexpr char const* parse(char const* in, char const* end) noexcept;
    ///
    /// void format(T const& value, format_output& out);
    template <typename T>
    struct formatter;

    /// Copy the source string to the destination buffer, but not extending
    /// past the provided buffer end pointer. Returns the pointer past the
    /// last character written.
    [[nodiscard]] constexpr char* copy_to(char* dest, char const* end, char const* source) noexcept;

    /// Copies length characters of source string to the destination
    /// buffer, but not extending past the provided buffer end pointer.
    /// Returns the pointer past the last character written.
    [[nodiscard]] constexpr char* copy_to_n(
        char* dest,
        char const* end,
        char const* source,
        std::size_t length) noexcept;

    /// Copies the provided character ch to the destination buffer, but not
    /// extending past the provided buffer end pointer. Returns the pointer
    /// past the last character written.
    [[nodiscard]] constexpr char* put(char* dest, char const* end, char ch) noexcept;

    /// Copies count copies of the charcter ch to the destination buffer, but
    /// not extending past the provided buffer end pointer. Returns the
    /// pointer past the last character written.
    [[nodiscard]] constexpr char* fill_n(char* dest, char const* end, char ch, std::size_t count) noexcept;

    /// Overload to support converting user-defined string types to format_string.
    template <typename StringT>
    constexpr format_string to_format_string(StringT const& value) noexcept;

    template <typename StringT>
    constexpr format_string::format_string(StringT const& string) noexcept : format_string(to_format_string(string)) {}

    /// Formats a string and arguments into dest, writing no more than count
    /// bytes. The destination will **NOT** be NUL-terminated. Returns a
    /// pointer to the last character written.
    template <typename... Args>
    [[nodiscard]] char* format_to_n(char* dest, std::size_t count, format_string format_str, Args const&... args);

    /// Formats a string and arguments into dest, writing no more than count
    /// bytes. The destination will **NOT** be NUL-terminated. Returns a
    /// pointer to the last character written.
    inline char* vformat_to_n(char* dest, std::size_t count, format_string format_str, format_args&& args);

    template <std::size_t N, typename... Args>
    char* format_to(char (&dest)[N], format_string format_str, Args const&... args);

    /// Formats a string and arguments into dest, writing no more than N
    /// bytes. The output will be NUL-terminated. Returns a pointer to the
    /// last character written, which will be the NUL byte itself.
    template <std::size_t N>
    char* vformat_to(char (&dest)[N], format_string format_str, format_args&& args);

    /// Returns the number of characters that would be written to a
    /// destination buffer (_excluding_ any terminating NUL) for the
    /// given format string and arguments
    template <typename... Args>
    [[nodiscard]] std::size_t format_length(format_string format_str, Args const&... args);

    [[nodiscard]] inline std::size_t vformat_length(format_string format_str, format_args&& args);

    /// Formats a value into dest, writing no more than N bytes. The output will
    /// be NUL-terminated. Returns a pointer to the last character written, which
    /// will be the NUL byte itself.
    template <typename ValueT, std::size_t N>
    char* format_value_to(char (&dest)[N], ValueT const& value, format_string spec = format_string{});

    /// Formats a value into dest, writing no more than count bytes. The
    /// destination will **NOT** be NUL-terminated. Returns a pointer to
    /// the last character written.
    template <typename ValueT>
    [[nodiscard]] char* format_value_to_n(
        char* dest,
        std::size_t count,
        ValueT const& value,
        format_string spec = format_string{});

    /// Calculates the length of the buffer required to hold the formatted value,
    /// excluded the trailing NUL byte.
    template <typename ValueT>
    [[nodiscard]] std::size_t format_value_length(ValueT const& value, format_string spec = format_string{});

    /// Constructs a format_args from a list of values.
    ///
    template <typename... Args>
    [[nodiscard]] constexpr auto make_format_args(Args const&... args) noexcept;

    namespace detail {
        struct format_spec {
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

        struct char_buffer;

        template <typename T>
        struct default_formatter {
            format_spec spec;
            char const* parse(char const* in, char const* end) noexcept;
            void format(T value, format_output& out) noexcept;
        };
    } // namespace detail

    template <>
    struct formatter<char> : detail::default_formatter<char> {};
    template <>
    struct formatter<bool> : detail::default_formatter<bool> {};

    template <>
    struct formatter<char*> : detail::default_formatter<char const*> {};
    template <>
    struct formatter<char const*> : detail::default_formatter<char const*> {};
    template <std::size_t N>
    struct formatter<char const[N]> : detail::default_formatter<detail::char_buffer> {};
    template <>
    struct formatter<format_string_view> : detail::default_formatter<format_string_view> {};

    template <>
    struct formatter<signed char> : detail::default_formatter<signed int> {};
    template <>
    struct formatter<unsigned char> : detail::default_formatter<unsigned int> {};
    template <>
    struct formatter<signed short> : detail::default_formatter<signed int> {};
    template <>
    struct formatter<unsigned short> : detail::default_formatter<unsigned int> {};
    template <>
    struct formatter<signed int> : detail::default_formatter<signed int> {};
    template <>
    struct formatter<unsigned int> : detail::default_formatter<unsigned int> {};
    template <>
    struct formatter<signed long> : detail::default_formatter<signed long> {};
    template <>
    struct formatter<unsigned long> : detail::default_formatter<unsigned long> {};
    template <>
    struct formatter<signed long long> : detail::default_formatter<signed long long> {};
    template <>
    struct formatter<unsigned long long> : detail::default_formatter<unsigned long long> {};

    template <>
    struct formatter<float> : detail::default_formatter<float> {};
    template <>
    struct formatter<double> : detail::default_formatter<double> {};

    template <>
    struct formatter<decltype(nullptr)> : detail::default_formatter<void const*> {};
    template <>
    struct formatter<void*> : detail::default_formatter<void const*> {};
    template <>
    struct formatter<void const*> : detail::default_formatter<void const*> {};
} // namespace NANOFMT_NS

#include "format.inl"

#endif // NANOFMT_FORMAT_H_
