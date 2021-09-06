// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include <type_traits>

namespace nanofmt::detail {
    using thunk_func = void (*)(void const* value, char const** spec, char const* end, buffer& buf);

    struct custom {
        thunk_func thunk = nullptr;
        void const* pointer = nullptr;
    };

    struct monostate {};

    enum class value_type {
        t_mono,
        t_int,
        t_uint,
        t_long,
        t_ulong,
        t_longlong,
        t_ulonglong,
        t_char,
        t_float,
        t_double,
        t_bool,
        t_cstring,
        t_stringview,
        t_voidptr,
        t_custom
    };

    struct value {
        constexpr value() noexcept : v_mono() {}
        constexpr value(signed char value) noexcept : v_int(value), type(value_type::t_int) {}
        constexpr value(unsigned char value) noexcept : v_unsigned(value), type(value_type::t_uint) {}
        constexpr value(short value) noexcept : v_int(value), type(value_type::t_int) {}
        constexpr value(unsigned short value) noexcept : v_unsigned(value), type(value_type::t_uint) {}
        constexpr value(int value) noexcept : v_int(value), type(value_type::t_int) {}
        constexpr value(unsigned value) noexcept : v_unsigned(value), type(value_type::t_uint) {}
        constexpr value(long value) noexcept : v_long(value), type(value_type::t_long) {}
        constexpr value(unsigned long value) noexcept : v_ulong(value), type(value_type::t_ulong) {}
        constexpr value(long long value) noexcept : v_longlong(value), type(value_type::t_longlong) {}
        constexpr value(unsigned long long value) noexcept : v_ulonglong(value), type(value_type::t_ulonglong) {}
        constexpr value(char value) noexcept : v_char(value), type(value_type::t_char) {}
        constexpr value(float value) noexcept : v_float(value), type(value_type::t_float) {}
        constexpr value(double value) noexcept : v_double(value), type(value_type::t_double) {}
        constexpr value(bool value) noexcept : v_bool(value), type(value_type::t_bool) {}
        constexpr value(char const* value) noexcept : v_cstring(value), type(value_type::t_cstring) {}
        constexpr value(string_view value) noexcept : v_stringview(value), type(value_type::t_stringview) {}
        constexpr value(decltype(nullptr) value) noexcept : v_voidptr(value), type(value_type::t_voidptr) {}
        constexpr value(void const* value) noexcept : v_voidptr(value), type(value_type::t_voidptr) {}
        constexpr value(custom value) noexcept : v_custom(value), type(value_type::t_custom) {}

        union {
            monostate v_mono;
            int v_int;
            unsigned v_unsigned;
            long v_long;
            unsigned long v_ulong;
            long long v_longlong;
            unsigned long long v_ulonglong;
            char v_char;
            float v_float;
            double v_double;
            bool v_bool;
            char const* v_cstring;
            string_view v_stringview;
            void const* v_voidptr;
            custom v_custom;
        };

        value_type type = value_type::t_mono;
    };

    template <size_t N>
    struct value_store {
        static constexpr size_t size = N;
        value values[N + 1 /* avoid size 0 */];
    };

    template <typename ValueT, typename FormatterT = formatter<ValueT>>
    void thunk_impl(void const* value, char const** in, char const* end, buffer& buf) {
        FormatterT fmt;
        if (in != nullptr) {
            *in = fmt.parse(*in, end);
        }
        fmt.format(*static_cast<ValueT const*>(value), buf);
    }

    // avoid explicitly pulling in <memory>
    template <typename T>
    constexpr T* addressof(T const& value) noexcept {
        return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(value)));
    }

    // avoid explicitly pulling in <utility>
    template <typename T>
    T&& declval();

    // helper for c++17
    template <class T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

    template <typename T, typename = void>
    struct has_formatter {
        static constexpr bool value = false;
    };
    template <typename T>
    struct has_formatter<
        T,
        std::void_t<decltype(declval<formatter<T>>().format(declval<T>(), declval<nanofmt::buffer&>()))>> {
        static constexpr bool value = true;
    };

    template <typename ValueT>
    constexpr auto make_format_value(ValueT const& value) noexcept {
        using T = std::decay_t<remove_cvref_t<ValueT>>;
        if constexpr (std::is_constructible_v<detail::value, T>) {
            return detail::value(value);
        }
        else if constexpr (has_formatter<T>::value) {
            detail::custom custom;
            custom.thunk = &nanofmt::detail::thunk_impl<T>;
            custom.pointer = addressof(value);
            return detail::value(custom);
        }
        else if constexpr (std::is_enum_v<T>) {
            return detail::value(static_cast<std::underlying_type_t<T>>(value));
        }
    };
} // namespace nanofmt::detail

namespace nanofmt {
    /// List of format args.
    ///
    /// Only use this type as a temporary value!
    ///
    struct format_args {
        template <size_t N>
        constexpr /*implicit*/ format_args(detail::value_store<N>&& values) noexcept
            : values(values.values)
            , count(N) {}

        void format(unsigned index, char const** in, char const* end, buffer& buf) const;

        detail::value const* values = nullptr;
        size_t count = 0;
    };

    template <typename... Args>
    constexpr auto make_format_args(Args const&... args) noexcept {
        return detail::value_store<sizeof...(Args)>{detail::make_format_value(args)...};
    }
} // namespace nanofmt
