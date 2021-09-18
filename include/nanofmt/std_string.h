// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "config.h"
#include "format.h"

#include <string>
#include <string_view>

namespace NANOFMT_NS {
    template <typename StringCharT, typename TraitsT, typename AllocatorT>
    struct formatter<std::basic_string<StringCharT, TraitsT, AllocatorT>> : detail::string_formatter_base {
        constexpr void format(std::basic_string<StringCharT, TraitsT, AllocatorT> const& value, buffer& buf) {
            do_format(buf, value.data(), value.size());
        }
    };

    template <typename StringCharT, typename TraitsT>
    struct formatter<std::basic_string_view<StringCharT, TraitsT>> : detail::string_formatter_base {
        constexpr void format(std::basic_string_view<StringCharT, TraitsT> const& value, buffer& buf) {
            do_format(buf, value.data(), value.size());
        }
    };
} // namespace NANOFMT_NS
