// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#ifndef NANOFMT_FORMAT_H_
#    error "format.inl is a private header; inglure format.h instead"
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace NANOFMT_NS {
    constexpr char* copy_to(char* dest, char const* end, char const* source) noexcept {
        char const* ptr = source;
        while (*ptr != 0 && dest != end)
            *dest++ = *ptr++;

        return dest;
    }

    constexpr char* copy_to_n(char* dest, char const* end, char const* source, std::size_t length) noexcept {
        char const* source_end = source + length;
        while (source != source_end && dest != end)
            *dest++ = *source++;

        return dest;
    }

    constexpr char* put(char* dest, char const* end, char ch) noexcept {
        if (dest != end) {
            *dest++ = ch;
        }
        return dest;
    }

    constexpr char* fill_n(char* dest, char const* end, char ch, std::size_t count) noexcept {
        char const pad_buffer[] = {ch, ch, ch, ch, ch, ch, ch, ch};
        constexpr std::size_t pad_length = sizeof pad_buffer;

        while (count >= pad_length) {
            dest = copy_to_n(dest, end, pad_buffer, pad_length);
            count -= pad_length;
        }
        return copy_to_n(dest, end, pad_buffer, count);
    }

    constexpr format_output& format_output::append(char const* const zstr) noexcept {
        char* const p = copy_to(pos, end, zstr);
        std::size_t const consumed = p - pos;
        pos = p;
        advance += consumed;
        advance += __builtin_strlen(zstr + consumed);
        return *this;
    }

    constexpr format_output& format_output::append(char const* source, std::size_t length) noexcept {
        pos = copy_to_n(pos, end, source, length);
        advance += length;
        return *this;
    }

    constexpr format_output& format_output::append(char ch) noexcept {
        pos = put(pos, end, ch);
        ++advance;
        return *this;
    }

    constexpr format_output& format_output::fill_n(char ch, std::size_t count) noexcept {
        pos = NANOFMT_NS::fill_n(pos, end, ch, count);
        advance += count;
        return *this;
    }

    constexpr format_output& format_output::advance_to(char* const p) noexcept {
        size_t const diff = p - pos;
        pos = p;
        advance += diff;
        return *this;
    }

    namespace detail {
        format_output vformat(format_output out, format_string format_str, format_args&& args);

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
            char const* chars = nullptr;
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

    format_output& vformat_to(format_output& out, format_string format_str, format_args&& args) {
        return out = detail::vformat(out, format_str, static_cast<format_args&&>(args));
    }

    [[nodiscard]] char* vformat_to_n(char* dest, std::size_t count, format_string format_str, format_args&& args) {
        return detail::vformat(format_output{dest, dest + count}, format_str, static_cast<format_args&&>(args)).pos;
    }

    template <typename... Args>
    [[nodiscard]] char* format_to_n(char* dest, std::size_t count, format_string format_str, Args const&... args) {
        return detail::vformat(format_output{dest, dest + count}, format_str, make_format_args(args...)).pos;
    }

    template <typename... Args>
    format_output& format_to(format_output& out, format_string format_str, Args const&... args) {
        return out = detail::vformat(out, format_str, make_format_args(args...));
    }

    template <std::size_t N, typename... Args>
    char* format_to(char (&dest)[N], format_string format_str, Args const&... args) {
        char* const pos =
            detail::vformat(format_output{dest, dest + (N - 1 /*NUL*/)}, format_str, make_format_args(args...)).pos;
        *pos = '\0';
        return pos;
    }

    template <typename... Args>
    [[nodiscard]] std::size_t format_length(format_string format_str, Args const&... args) {
        return detail::vformat(format_output(nullptr, 0), format_str, make_format_args(args...)).advance;
    }

    [[nodiscard]] std::size_t vformat_length(format_string format_str, format_args&& args) {
        return detail::vformat(format_output{}, format_str, static_cast<format_args&&>(args)).advance;
    }

    namespace detail {
        template <typename ValueT>
        format_output format_value(format_output out, ValueT const& value, format_string spec) {
            formatter<ValueT> fmt;
            if (char const* const spec_end = fmt.parse(spec.begin, spec.end); spec_end != spec.end) {
                return out;
            }
            fmt.format(value, out);
            return out;
        }
    } // namespace detail

    template <typename ValueT>
    char* format_value_to(format_output& out, ValueT const& value, format_string spec) {
        return detail::format_value(out, value, spec).pos;
    }

    template <typename ValueT>
    [[nodiscard]] char* format_value_to_n(char* dest, std::size_t count, ValueT const& value, format_string spec) {
        return detail::format_value(format_output{dest, dest + count}, value, spec).pos;
    }

    template <typename ValueT, std::size_t N>
    char* format_value_to(char (&dest)[N], ValueT const& value, format_string spec) {
        char* const pos = detail::format_value(format_output{dest, dest + (N - 1 /*NUL*/)}, value, spec).pos;
        *pos = '\0';
        return pos;
    }

    template <typename ValueT>
    [[nodiscard]] std::size_t format_value_length(ValueT const& value, format_string spec) {
        return detail::format_value(format_output{}, value, spec).advance;
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
            void (*thunk)(void const* value, char const** spec, char const* end, format_output& out) = nullptr;
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
                decltype(formatter<T>{}.format(declval<T>(), format_output{}))>> {
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
            using type = unsigned int;
        };
        template <>
        struct value_type_map<signed short> {
            using type = signed int;
        };
        template <>
        struct value_type_map<unsigned short> {
            using type = unsigned int;
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
                custom.thunk = +[](void const* value, char const** in, char const* end, format_output& out) {
                    formatter<ValueT> fmt;
                    if (in != nullptr) {
                        *in = fmt.parse(*in, end);
                    }
                    fmt.format(*static_cast<ValueT const*>(value), out);
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

        void format(unsigned index, char const** in, char const* end, format_output& out) const;

        format_arg const* values = nullptr;
        size_t count = 0;
    };

    template <typename... Args>
    [[nodiscard]] constexpr auto make_format_args(Args const&... args) noexcept {
        return format_arg_store<sizeof...(Args)>{detail::make_format_arg(args)...};
    }
} // namespace NANOFMT_NS

#endif
