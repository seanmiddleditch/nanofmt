// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "nanofmt/format.h"
#include "nanofmt/to_chars.h"

#include <cstring>
#include <iostream>

namespace NANOFMT_NS::test {
    template <size_t N>
    struct string_result {
        char buffer[N] = {};
        size_t size = 0;

        friend bool operator==(string_result const& lhs, char const* zstr) noexcept {
            size_t const zlen = std::strlen(zstr);
            if (zlen != lhs.size)
                return false;
            return std::memcmp(lhs.buffer, zstr, lhs.size) == 0;
        }

        friend std::ostream& operator<<(std::ostream& os, string_result const& val) {
            return os.put('"').write(val.buffer, val.size).put('"');
        }
    };

    template <size_t N = 2048, typename FormatT, typename... ArgsT>
    auto sformat(FormatT&& format_str, ArgsT&&... args) {
        string_result<N> result;
        char const* const end = NANOFMT_NS::format_to_n(
            result.buffer,
            sizeof result.buffer,
            std::forward<FormatT>(format_str),
            std::forward<ArgsT>(args)...);
        result.size = end - result.buffer;
        return result;
    }

    template <size_t N = 2048, typename ValueT, typename... ArgsT>
    auto to_string(ValueT const& value, ArgsT&&... args) {
        string_result<N> result;
        char const* const end =
            NANOFMT_NS::to_chars(result.buffer, result.buffer + sizeof result.buffer, value, args...);
        result.size = end - result.buffer;
        return result;
    }
} // namespace NANOFMT_NS::test

// wow this is annoying; see https://github.com/isocpp/CppCoreGuidelines/issues/1173
//
#if defined(__has_cpp_attribute)
#    if __has_cpp_attribute(gsl::suppress)
#        if defined(__clang__)
// clang-format off
#        define NANOFMT_GSL_SUPPRESS(rule) [[gsl::suppress(#rule)]]
// clang-format on
#        endif
#    elif defined(_MSC_VER)
#        define NANOFMT_GSL_SUPPRESS(rule) [[gsl::suppress(rule)]]
#    endif
#endif
#if !defined(NANOFMT_GSL_SUPPRESS)
#    define NANOFMT_GSL_SUPPRESS(rule)
#endif
