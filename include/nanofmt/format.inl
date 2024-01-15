// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#ifndef NANOFMT_FORMAT_H_
#    error "format.inl is a private header; include format.h instead"
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace NANOFMT_NS {
    namespace detail {
        struct vformat_result {
            char* pos = nullptr;
            std::size_t advance = 0;
        };

        vformat_result vformat(char* dest, char const* end, format_string format_str, format_args args);

        // avoid explicitly pulling in <utility>
        template <typename T>
        const T& declval() noexcept;

        template <typename ValueT>
        constexpr format_arg make_format_arg(ValueT const& value) noexcept;

        template <typename T>
        struct value_type_map;

        template <typename T>
        using enable_if_format_string =
            std::enable_if_t<std::is_same_v<::NANOFMT_NS::format_string, decltype(to_format_string(declval<T>()))>>;

        struct char_buffer {
            char const* chars = nullptr;
            std::size_t max_length = 0;
        };

    } // namespace detail

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
            void (*thunk)(void const* value, char const** spec, char const* end, format_context& ctx) = nullptr;
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

        constexpr bool is_integer_type() const noexcept {
            return tag >= type::t_int && tag <= type::t_ulonglong;
        }
        constexpr bool is_string_type() const noexcept {
            return tag == type::t_cstring;
        }

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

    template <size_t N>
    struct format_arg_store {
        static constexpr size_t size = N;
        format_arg values[N + 1 /* avoid size 0 */];
    };

    struct format_args {
        template <size_t N>
        constexpr /*implicit*/ format_args(format_arg_store<N>&& values) noexcept : values(values.values)
                                                                                  , count(N) {}

        void format(unsigned index, char const** in, char const* end, format_context& ctx) const;

        format_arg const* values = nullptr;
        size_t count = 0;
    };

    template <typename... Args>
    [[nodiscard]] constexpr auto make_format_args(Args const&... args) noexcept {
        return format_arg_store<sizeof...(Args)>{detail::make_format_arg(args)...};
    }

    struct format_string {
        constexpr format_string() noexcept = default;
        constexpr format_string(char const* string, std::size_t length) noexcept;
        template <std::size_t N>
        constexpr /*implicit*/ format_string(char const (&str)[N]) noexcept;
        constexpr explicit format_string(char const* const zstr) noexcept;

        template <typename StringT, typename = detail::enable_if_format_string<StringT>>
        constexpr /*implicit*/ format_string(StringT const& string) noexcept;

        char const* begin = nullptr;
        char const* end = nullptr;
    };

    struct format_parse_context {
        const char* pos = nullptr;
        const char* sentinel = nullptr;

        constexpr const char* begin() const noexcept {
            return pos;
        }
        constexpr const char* end() const noexcept {
            return sentinel;
        }

        constexpr void advance_to(const char* to) noexcept {
            pos = to;
        }
    };

    struct format_context {
        char* pos = nullptr;
        char const* end = nullptr;
        std::size_t advance = 0;

        constexpr format_context& append(char const* zstr) noexcept;
        constexpr format_context& append(char const* source, std::size_t length) noexcept;

        constexpr format_context& put(char ch) noexcept;

        constexpr format_context& fill_n(char ch, std::size_t count) noexcept;

        template <typename... Args>
        format_context& format(format_string fmt, Args const&... args);

        inline format_context& vformat(format_string fmt, format_args args);

        constexpr format_context& advance_to(char* p) noexcept;

        constexpr char* out() const noexcept {
            return pos;
        }
    };

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

    constexpr std::size_t strnlen(char const* buffer, std::size_t count) noexcept {
        char const* const end = buffer + count;
        for (char const* pos = buffer; pos != end; ++pos)
            if (*pos == '\0')
                return pos - buffer;
        return count;
    }

    constexpr format_context& format_context::append(char const* const zstr) noexcept {
        char* const p = copy_to(pos, end, zstr);
        std::size_t const consumed = p - pos;
        pos = p;
        advance += consumed;
        advance += __builtin_strlen(zstr + consumed);
        return *this;
    }

    constexpr format_context& format_context::append(char const* source, std::size_t length) noexcept {
        pos = copy_to_n(pos, end, source, length);
        advance += length;
        return *this;
    }

    constexpr format_context& format_context::put(char ch) noexcept {
        pos = ::NANOFMT_NS::put(pos, end, ch);
        ++advance;
        return *this;
    }

    constexpr format_context& format_context::fill_n(char ch, std::size_t count) noexcept {
        pos = ::NANOFMT_NS::fill_n(pos, end, ch, count);
        advance += count;
        return *this;
    }

    template <typename... Args>
    format_context& format_context::format(format_string fmt, Args const&... args) {
        const detail::vformat_result result = detail::vformat(pos, end, fmt, ::NANOFMT_NS::make_format_args(args...));
        pos = result.pos;
        advance += result.advance;
        return *this;
    }

    format_context& format_context::vformat(format_string fmt, format_args args) {
        const detail::vformat_result result = detail::vformat(pos, end, fmt, static_cast<format_args&&>(args));
        pos = result.pos;
        advance += result.advance;
        return *this;
    }

    constexpr format_context& format_context::advance_to(char* const p) noexcept {
        size_t const diff = p - pos;
        pos = p;
        advance += diff;
        return *this;
    }

    struct format_string_view {
        char const* string = nullptr;
        std::size_t length = 0;
    };

    constexpr format_string::format_string(char const* string, std::size_t length) noexcept
        : begin(string)
        , end(string + length) {}

    template <std::size_t N>
    constexpr format_string::format_string(char const (&str)[N]) noexcept
        : begin(str)
        , end(begin + __builtin_strlen(begin)) {}

    constexpr format_string::format_string(char const* const zstr) noexcept
        : begin(zstr)
        , end(begin + __builtin_strlen(begin)) {}

    template <typename StringT, typename>
    constexpr format_string::format_string(StringT const& string) noexcept : format_string(to_format_string(string)) {}

    template <typename StringT>
    constexpr format_string to_format_string(StringT const& value) noexcept {
        return {value.data(), value.size()};
    }

    [[nodiscard]] char* vformat_to_n(char* dest, std::size_t count, format_string format_str, format_args args) {
        return detail::vformat(dest, dest + count, format_str, static_cast<format_args&&>(args)).pos;
    }

    template <typename... Args>
    [[nodiscard]] char* format_to_n(char* dest, std::size_t count, format_string format_str, Args const&... args) {
        return detail::vformat(dest, dest + count, format_str, ::NANOFMT_NS::make_format_args(args...)).pos;
    }

    template <std::size_t N, typename... Args>
    char* format_to(char (&dest)[N], format_string format_str, Args const&... args) {
        const detail::vformat_result result =
            detail::vformat(dest, dest + (N - 1 /*NUL*/), format_str, ::NANOFMT_NS::make_format_args(args...));
        *result.pos = '\0';
        return result.pos;
    }

    template <std::size_t N>
    char* vformat_to(char (&dest)[N], format_string format_str, format_args args) {
        const detail::vformat_result result =
            detail::vformat(dest, dest + (N - 1 /*NUL*/), format_str, args);
        *result.pos = '\0';
        return result.pos;
    }

    template <typename... Args>
    [[nodiscard]] char* format_append_to_n(
        char* dest,
        std::size_t count,
        format_string format_str,
        Args const&... args) {
        std::size_t const start = ::NANOFMT_NS::strnlen(dest, count);
        return detail::vformat(
                   dest + start, dest + count,
                   format_str,
                   ::NANOFMT_NS::make_format_args(args...))
            .pos;
    }

    template <typename... Args>
    [[nodiscard]] char* vformat_append_to_n(char* dest, std::size_t count, format_string format_str, format_args args) {
        std::size_t const start = ::NANOFMT_NS::strnlen(dest, count);
        return detail::vformat(dest + start, dest + count, format_str, args).pos;
    }

    template <std::size_t N, typename... Args>
    char* format_append_to(char (&dest)[N], format_string format_str, Args const&... args) {
        return vformat_append_to(dest, format_str, ::NANOFMT_NS::make_format_args(args...));
    }

    template <std::size_t N, typename... Args>
    char* vformat_append_to(char (&dest)[N], format_string format_str, format_args args) {
        std::size_t const start = ::NANOFMT_NS::strnlen(dest, N);
        if (start == N) {
            return dest + N;
        }
        char* const pos = detail::vformat(dest + start, dest + (N - 1 /*NUL*/), format_str, args).pos;
        *pos = '\0';
        return pos;
    }

    template <typename... Args>
    [[nodiscard]] std::size_t format_length(format_string format_str, Args const&... args) {
        return detail::vformat(nullptr, nullptr, format_str, ::NANOFMT_NS::make_format_args(args...)).advance;
    }

    [[nodiscard]] std::size_t vformat_length(format_string format_str, format_args args) {
        return detail::vformat(nullptr, nullptr, format_str, static_cast<format_args&&>(args)).advance;
    }

    namespace detail {
        template <typename T>
        using has_formatter = std::is_default_constructible<::NANOFMT_NS::formatter<T>>;

        template <typename T>
        constexpr bool always_false_v = false;

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
                custom.thunk = +[](void const* value, char const** in, char const* end, format_context& ctx) {
                    formatter<ValueT> fmt;
                    if (in != nullptr) {
                        format_parse_context pctx{*in, end};
                        *in = fmt.parse(pctx);
                    }
                    fmt.format(*static_cast<ValueT const*>(value), ctx);
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
                static_assert(always_false_v<ValueT>, "Type has no nanofmt::formatter<> specialization");
            }
        }
    } // namespace detail
} // namespace NANOFMT_NS

#endif
