// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#ifndef NANOFMT_STD_STRING_H_
#define NANOFMT_STD_STRING_H_ 1
#pragma once

#include "config.h"
#include "format.h"

#include <string>
#include <string_view>

namespace NANOFMT_NS {
    template <typename StringCharT, typename TraitsT, typename AllocatorT>
    struct formatter<std::basic_string<StringCharT, TraitsT, AllocatorT>> : formatter<format_string_view> {
        constexpr void format(std::basic_string<StringCharT, TraitsT, AllocatorT> const& value, format_context& ctx) {
            formatter<format_string_view>::format({value.data(), value.size()}, ctx);
        }
    };

    template <typename StringCharT, typename TraitsT>
    struct formatter<std::basic_string_view<StringCharT, TraitsT>> : formatter<format_string_view> {
        constexpr void format(std::basic_string_view<StringCharT, TraitsT> const& value, format_context& ctx) {
            formatter<format_string_view>::format({value.data(), value.size()}, ctx);
        }
    };
} // namespace NANOFMT_NS

#endif
