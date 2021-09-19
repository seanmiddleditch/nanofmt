// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "config.h"

#include <cstddef>

namespace NANOFMT_NS {
    struct format_buffer;

    [[nodiscard]] constexpr char* copy_to(char* buffer, char const* end, char const* source) noexcept;
    [[nodiscard]] constexpr char* copy_to_n(
        char* buffer,
        char const* end,
        char const* source,
        std::size_t length) noexcept;
    [[nodiscard]] constexpr char* put(char* buffer, char const* end, char ch) noexcept;
    [[nodiscard]] constexpr char* fill_n(char* buffer, char const* end, char ch, std::size_t count) noexcept;

    /// Small wrapper about a character sequence.
    ///
    /// Counts the number of characters that are written to the buffer.
    ///
    /// Use advance_to to update the pos pointer to ensure the advance field
    /// is updated appropriately.
    ///
    struct format_buffer {
        constexpr format_buffer() noexcept = default;
        constexpr format_buffer(char* const dest, std::size_t count) noexcept : pos(dest), end(dest + count) {}

        constexpr format_buffer& append(char const* const zstr) noexcept {
            char* const p = copy_to(pos, end, zstr);
            std::size_t const consumed = p - pos;
            advance += consumed;
            advance += __builtin_strlen(zstr + consumed);
            pos = p;
            return *this;
        }

        constexpr format_buffer& append(char const* source, std::size_t length) noexcept {
            advance += length;
            pos = copy_to_n(pos, end, source, length);
            return *this;
        }

        constexpr format_buffer& append(char ch) noexcept {
            ++advance;
            pos = put(pos, end, ch);
            return *this;
        }

        constexpr format_buffer& fill_n(char ch, std::size_t count) noexcept {
            advance += count;
            pos = NANOFMT_NS::fill_n(pos, end, ch, count);
            return *this;
        }

        constexpr format_buffer& advance_to(char* const p) noexcept {
            advance += p - pos;
            pos = p;
            return *this;
        }

        char* pos = nullptr;
        char const* end = nullptr;
        std::size_t advance = 0;
    };

    /// Copy the source string to the destination buffer, but not extending
    /// past the provided buffer end pointer. Returns the pointer past the
    /// last character written.
    constexpr char* copy_to(char* buffer, char const* end, char const* source) noexcept {
        char const* ptr = source;
        while (*ptr != 0 && buffer != end)
            *buffer++ = *ptr++;

        return buffer;
    }

    /// Copies up to length characters of source string to the destination
    /// buffer, but not extending past the provided buffer end pointer.
    /// Returns the pointer past the last character written.
    constexpr char* copy_to_n(char* buffer, char const* end, char const* source, std::size_t length) noexcept {
        char const* source_end = source + length;
        while (source != source_end && buffer != end)
            *buffer++ = *source++;

        return buffer;
    }

    /// Copies the provided character ch to the destination buffer, but not
    /// extending past the provided buffer end pointer. Returns the pointer
    /// past the last character written.
    constexpr char* put(char* buffer, char const* end, char ch) noexcept {
        if (buffer != end) {
            *buffer++ = ch;
        }
        return buffer;
    }

    /// Copies count copies of the charcter ch to the destination buffer, but
    /// not extending past the provided buffer end pointer. Returns the
    /// pointer past the last character written.
    constexpr char* fill_n(char* buffer, char const* end, char ch, std::size_t count) noexcept {
        char const pad_buffer[] = {ch, ch, ch, ch, ch, ch, ch, ch};
        constexpr std::size_t pad_length = sizeof pad_buffer;

        while (count >= pad_length) {
            buffer = copy_to_n(buffer, end, pad_buffer, pad_length);
            count -= pad_length;
        }
        return copy_to_n(buffer, end, pad_buffer, count);
    }
} // namespace NANOFMT_NS
