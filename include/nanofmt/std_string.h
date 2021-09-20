// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "config.h"
#include "format.h"

#include <string>

#if __cpp_lib_string_view
#    include <string_view>
#endif

namespace NANOFMT_NS {
    template <typename StringCharT, typename TraitsT, typename AllocatorT>
    struct formatter<std::basic_string<StringCharT, TraitsT, AllocatorT>> : formatter<format_string_view> {
        constexpr void format(std::basic_string<StringCharT, TraitsT, AllocatorT> const& value, format_output& out) {
            formatter<format_string_view>::format({value.data(), value.size()}, out);
        }
    };

#if __cpp_lib_string_view
    template <typename StringCharT, typename TraitsT>
    struct formatter<std::basic_string_view<StringCharT, TraitsT>> : formatter<format_string_view> {
        constexpr void format(std::basic_string_view<StringCharT, TraitsT> const& value, format_output& out) {
            formatter<format_string_view>::format({value.data(), value.size()}, out);
        }
    };
#endif
} // namespace NANOFMT_NS
