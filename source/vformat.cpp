// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "parse_utils.h"
#include "nanofmt/format.h"

namespace NANOFMT_NS {
    char* detail::vformat(buffer& buf, format_string format_str, format_args&& args) {
        int arg_next_index = 0;
        bool arg_auto_index = true;

        char const* input = format_str.begin;
        char const* input_begin = input;
        char const* const input_end = format_str.end;

        while (input != input_end) {
            if (*input != '{') {
                ++input;
                continue;
            }

            // write out the string so far, since we don't write characters immediately
            buf.append(input_begin, input - input_begin);

            ++input; // swallow the {

            // if we hit the end of the input, we have an incomplete format, and nothing else we can do
            if (input == input_end) {
                return buf.pos;
            }

            // if we just have another { then take it as a literal character by starting our next begin here,
            // so it'll get written next time we write out the begin; nothing else to do for formatting here
            if (*input == '{') {
                input_begin = input++;
                continue;
            }

            // determine argument
            int arg_index = 0;
            if (arg_index = detail::parse_nonnegative(input, input_end); arg_index != -1) {
                arg_auto_index = false;
            }
            else if (arg_auto_index) {
                arg_index = arg_next_index++;
            }
            else {
                // we received a non-explicit index after an explicit index
                return buf.pos;
            }

            // extract formatter specification/arguments
            char const** spec = nullptr;
            if (input != input_end && *input == ':') {
                spec = &++input;
            }

            // format the value
            args.format(arg_index, spec, input_end, buf);
            if (spec != nullptr) {
                input = *spec;
            }

            // consume parse specification, and any trailing }
            if (input != input_end && *input == '}') {
                ++input;
            }

            // mark where the next text run will begin
            input_begin = input;
        }

        // write out tail end of format string
        buf.append(input_begin, input - input_begin);
        return buf.pos;
    }

    void format_args::format(unsigned index, char const** in, char const* end, buffer& buf) const {
        using types = format_value::type;

        if (index >= count) {
            return;
        }

        format_value const& value = values[index];

        auto invoke = [in, end, &buf](auto value) {
            formatter<decltype(value)> fmt;
            if (in != nullptr) {
                *in = fmt.parse(*in, end);
            }
            fmt.format(value, buf);
        };

        switch (value.tag) {
            case types::t_mono:
                return;
            case types::t_char:
                return invoke(value.v_char);
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
                return value.v_custom.thunk(value.v_custom.value, in, end, buf);
        }
    }
} // namespace NANOFMT_NS
