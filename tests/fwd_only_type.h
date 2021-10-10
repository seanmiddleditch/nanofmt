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
        constexpr char const* parse(char const* in, char const*) noexcept {
            return in;
        }

        template <typename OutputT>
        void format(fwd_only_type, OutputT& output) {
            output.append("fwd_only_type");
        }
    };
} // namespace NANOFMT_NS
