// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#ifndef NANOFMT_FORMAT_H_
#    error "format.inl is a private header; inglure format.h instead"
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace NANOFMT_NS {
    namespace detail {
        char* vformat(format_buffer& buf, format_string format_str, format_args&& args);

        // avoid explicitly pulling in <utility>
        template <typename T>
        const T& declval() noexcept;

        template <typename ValueT>
        constexpr format_arg make_format_arg(ValueT const& value) noexcept;

        template <typename T, typename = void>
        struct has_formatter;
        template <typename T>
        struct value_type_map;

        struct char_buffer {
            char const* buffer = nullptr;
            std::size_t max_length = 0;
        };
    } // namespace detail

    struct format_string_view {
        char const* string = nullptr;
        std::size_t length = 0;
    };

    template <typename StringT>
    constexpr format_string to_format_string(StringT const& value) noexcept {
        return {value.data(), value.size()};
    }

    char* vformat_to(format_buffer& buf, format_string format_str, format_args&& args) {
        return detail::vformat(buf, format_str, static_cast<format_args&&>(args));
    }

    char* vformat_to_n(char* dest, std::size_t count, format_string format_str, format_args&& args) {
        format_buffer buf(dest, count);
        return detail::vformat(buf, format_str, static_cast<format_args&&>(args));
    }

    template <typename... Args>
    char* format_to_n(char* dest, std::size_t count, format_string format_str, Args const&... args) {
        format_buffer buf(dest, count);
        return detail::vformat(buf, format_str, make_format_args(args...));
    }

    template <typename... Args>
    char* format_to(format_buffer& buf, format_string format_str, Args const&... args) {
        return detail::vformat(buf, format_str, make_format_args(args...));
    }

    template <std::size_t N, typename... Args>
    char* format_to(char (&dest)[N], format_string format_str, Args const&... args) {
        format_buffer buf(dest, N - 1 /*NUL*/);
        char* const end = detail::vformat(buf, format_str, make_format_args(args...));
        *end = '\0';
        return end;
    }

    template <typename... Args>
    std::size_t format_size(format_string format_str, Args const&... args) {
        format_buffer buf(nullptr, 0);
        detail::vformat(buf, format_str, make_format_args(args...));
        return buf.advance;
    }

    std::size_t vformat_size(format_string format_str, format_args&& args) {
        format_buffer buf(nullptr, 0);
        detail::vformat(buf, format_str, static_cast<format_args&&>(args));
        return buf.advance;
    }

    template <typename ValueT>
    char* format_value_to(format_buffer& buf, ValueT const& value, format_string spec) {
        formatter<ValueT> fmt;
        if (char const* const spec_end = fmt.parse(spec.begin, spec.end); spec_end != spec.end) {
            return buf.pos;
        }
        fmt.format(value, buf);
        return buf.pos;
    }

    template <typename ValueT>
    char* format_value_to_n(char* dest, std::size_t count, ValueT const& value, format_string spec) {
        format_buffer buf(dest, count);
        return format_value_to(buf, value, spec);
    }

    template <typename ValueT, std::size_t N>
    char* format_value_to(char (&dest)[N], ValueT const& value, format_string spec) {
        format_buffer buf(dest, N - 1 /*NUL*/);
        char* const end = format_value_to(buf, value, spec);
        *end = '\0';
        return end;
    }

    template <typename ValueT>
    std::size_t format_value_size(ValueT const& value, format_string spec) {
        format_buffer buf(nullptr, 0);
        format_value_to(buf, value, spec);
        return buf.advance;
    }

    struct format_arg {
        enum class type {
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
            t_voidptr,
            t_custom
        };

        struct custom {
            void (*thunk)(void const* value, char const** spec, char const* end, format_buffer& buf) = nullptr;
            void const* value = nullptr;
        };

        constexpr format_arg() noexcept : v_int(0) {}
        constexpr format_arg(int value) noexcept : v_int(value), tag(type::t_int) {}
        constexpr format_arg(unsigned value) noexcept : v_unsigned(value), tag(type::t_uint) {}
        constexpr format_arg(long value) noexcept : v_long(value), tag(type::t_long) {}
        constexpr format_arg(unsigned long value) noexcept : v_ulong(value), tag(type::t_ulong) {}
        constexpr format_arg(long long value) noexcept : v_longlong(value), tag(type::t_longlong) {}
        constexpr format_arg(unsigned long long value) noexcept : v_ulonglong(value), tag(type::t_ulonglong) {}
        constexpr format_arg(char value) noexcept : v_char(value), tag(type::t_char) {}
        constexpr format_arg(float value) noexcept : v_float(value), tag(type::t_float) {}
        constexpr format_arg(double value) noexcept : v_double(value), tag(type::t_double) {}
        constexpr format_arg(bool value) noexcept : v_bool(value), tag(type::t_bool) {}
        constexpr format_arg(char const* value) noexcept : v_cstring(value), tag(type::t_cstring) {}
        constexpr format_arg(void const* value) noexcept : v_voidptr(value), tag(type::t_voidptr) {}
        constexpr format_arg(custom value) noexcept : v_custom(value), tag(type::t_custom) {}

        union {
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
            void const* v_voidptr;
            custom v_custom;
        };

        type tag = type::t_mono;
    };

    namespace detail {
        template <typename T, typename>
        struct has_formatter {
            static constexpr bool value = false;
        };
        template <typename T>
        struct has_formatter<
            T,
            std::void_t<
                decltype(formatter<T>{}.parse("", "")),
                decltype(formatter<T>{}.format(declval<T>(), format_buffer{}))>> {
            static constexpr bool value = true;
        };

        template <typename T>
        struct value_type_map {
            using type = T;
        };
        template <>
        struct value_type_map<signed char> {
            using type = signed int;
        };
        template <>
        struct value_type_map<unsigned char> {
            using type = signed int;
        };
        template <>
        struct value_type_map<signed short> {
            using type = signed int;
        };
        template <>
        struct value_type_map<unsigned short> {
            using type = signed int;
        };
        template <>
        struct value_type_map<decltype(nullptr)> {
            using type = void const*;
        };

        template <typename ValueT>
        constexpr format_arg make_format_arg(ValueT const& value) noexcept {
            using MappedT = typename detail::value_type_map<std::decay_t<ValueT>>::type;
            if constexpr (std::is_constructible_v<format_arg, MappedT>) {
                return (MappedT)(value);
            }
            else if constexpr (detail::has_formatter<ValueT>::value) {
                format_arg::custom custom;
                custom.thunk = +[](void const* value, char const** in, char const* end, format_buffer& buf) {
                    formatter<ValueT> fmt;
                    if (in != nullptr) {
                        *in = fmt.parse(*in, end);
                    }
                    fmt.format(*static_cast<ValueT const*>(value), buf);
                };
                // this is basically std::addressof, but we want to avoid pulling in <memory> as a dependency
                custom.value =
                    reinterpret_cast<ValueT*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(value)));
                return custom;
            }
            else if constexpr (std::is_enum_v<ValueT>) {
                return static_cast<typename detail::value_type_map<std::underlying_type_t<ValueT>>::type>(value);
            }
            else {
                return {};
            }
        }
    } // namespace detail

    template <size_t N>
    struct format_arg_store {
        static constexpr size_t size = N;
        format_arg values[N + 1 /* avoid size 0 */];
    };

    struct format_args {
        template <size_t N>
        constexpr /*implicit*/ format_args(format_arg_store<N>&& values) noexcept : values(values.values)
                                                                                  , count(N) {}

        void format(unsigned index, char const** in, char const* end, format_buffer& buf) const;

        format_arg const* values = nullptr;
        size_t count = 0;
    };

    template <typename... Args>
    constexpr auto make_format_args(Args const&... args) noexcept {
        return format_arg_store<sizeof...(Args)>{detail::make_format_arg(args)...};
    }
} // namespace NANOFMT_NS

#endif
