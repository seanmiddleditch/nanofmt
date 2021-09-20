// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "config.h"
#include "format.h"

#include <string>
#include <string_view>

namespace NANOFMT_NS {
    template <typename StringCharT, typename TraitsT, typename AllocatorT>
    struct formatter<std::basic_string<StringCharT, TraitsT, AllocatorT>> : formatter<format_string_view> {
        constexpr void format(std::basic_string<StringCharT, TraitsT, AllocatorT> const& value, format_output& buffer) {
            formatter<format_string_view>::format({value.data(), value.size()}, buffer);
        }
    };

    template <typename StringCharT, typename TraitsT>
    struct formatter<std::basic_string_view<StringCharT, TraitsT>> : formatter<format_string_view> {
        constexpr void format(std::basic_string_view<StringCharT, TraitsT> const& value, format_output& buffer) {
            formatter<format_string_view>::format({value.data(), value.size()}, buffer);
        }
    };
} // namespace NANOFMT_NS
