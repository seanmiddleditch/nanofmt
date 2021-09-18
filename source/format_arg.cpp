// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "nanofmt/format.h"
#include "nanofmt/format_arg.h"

namespace NANOFMT_NS {
    void format_args::format(unsigned index, char const** in, char const* end, buffer& buf) const {
        using types = detail::value_type;

        if (index >= count) {
            return;
        }

        detail::value const& value = values[index];

        auto invoke = [in, end, &buf](auto value) {
            formatter<decltype(value)> fmt;
            if (in != nullptr) {
                *in = fmt.parse(*in, end);
            }
            fmt.format(value, buf);
        };

        switch (value.type) {
            case types::t_mono:
                return;
            case types::t_char:
                buf.append(value.v_char);
                return;
            case types::t_int:
                return invoke(value.v_int);
            case types::t_uint:
                return invoke(value.v_unsigned);
            case types::t_long:
                return invoke(value.v_long);
            case types::t_ulong:
                return invoke(value.v_ulong);
            case types::t_longlong:
                return invoke(value.v_longlong);
            case types::t_ulonglong:
                return invoke(value.v_ulonglong);
            case types::t_float:
                return invoke(value.v_float);
            case types::t_double:
                return invoke(value.v_double);
            case types::t_bool:
                return invoke(value.v_bool);
            case types::t_cstring:
                return invoke(value.v_cstring);
            case types::t_voidptr:
                return invoke(value.v_voidptr);
            case types::t_custom:
                return value.v_custom.thunk(value.v_custom.pointer, in, end, buf);
        }
    }
} // namespace NANOFMT_NS
