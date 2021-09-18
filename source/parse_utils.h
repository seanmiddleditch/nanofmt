// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

namespace NANOFMT_NS::detail {
    constexpr int parse_nonnegative(char const*& start, char const* end) noexcept {
        if (start == end) {
            return -1;
        }

        if (*start == '0') {
            ++start;
            return 0;
        }

        // there must be at least one non-zero digit
        if (!(*start >= '1' && *start <= '9')) {
            return -1;
        }

        int result = 0;
        while (start != end && *start >= '0' && *start <= '9') {
            result *= 10;
            result += *start - '0';
            ++start;
        }
        return result;
    }
} // namespace NANOFMT_NS::detail
