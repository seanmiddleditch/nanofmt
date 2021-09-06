// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include <string>
#include <string_view>

namespace nanofmt {
    template <typename StringCharT, typename TraitsT, typename AllocatorT>
    struct formatter<std::basic_string<StringCharT, TraitsT, AllocatorT>> : formatter<string_view> {
        constexpr void format(std::basic_string<StringCharT, TraitsT, AllocatorT> const& value, buffer& buf) {
            formatter<string_view>::format({value.data(), value.size()}, buf);
        }
    };

    template <typename StringCharT, typename TraitsT>
    struct formatter<std::basic_string_view<StringCharT, TraitsT>> : formatter<string_view> {
        constexpr void format(std::basic_string_view<StringCharT, TraitsT> const& value, buffer& buf) {
            formatter<string_view>::format({value.data(), value.size()}, buf);
        }
    };
} // namespace nanofmt
