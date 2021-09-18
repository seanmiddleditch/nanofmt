// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "config.h"
#include "format.h"

#include <string>
#include <string_view>

namespace NANOFMT_NS {
    template <typename StringCharT, typename TraitsT, typename AllocatorT>
    struct formatter<std::basic_string<StringCharT, TraitsT, AllocatorT>> : detail::string_view_formatter_base {
        constexpr void format(std::basic_string<StringCharT, TraitsT, AllocatorT> const& value, buffer& buf) {
            string_view_formatter_base::format(value.data(), value.size(), buf);
        }
    };

    template <typename StringCharT, typename TraitsT>
    struct formatter<std::basic_string_view<StringCharT, TraitsT>> : detail::string_view_formatter_base {
        constexpr void format(std::basic_string_view<StringCharT, TraitsT> const& value, buffer& buf) {
            string_view_formatter_base::format(value.data(), value.size(), buf);
        }
    };
} // namespace NANOFMT_NS
