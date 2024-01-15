// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#if defined(NANOFMT_FORMAT_H_)
#    error "fwd_only_type.h should be included before format.h for the test to be correct"
#endif

#include "nanofmt/forward.h"

struct fwd_only_type {};

namespace NANOFMT_NS {
    template <>
    struct formatter<fwd_only_type> {
        template <typename ContextT = format_parse_context>
        constexpr char const* parse(ContextT& ctx) noexcept {
            return ctx.begin();
        }

        template <typename ContextT = format_context>
        void format(fwd_only_type, ContextT& ctx) {
            ctx.append("fwd_only_type");
        }
    };
} // namespace NANOFMT_NS
