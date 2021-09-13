// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include <cstddef>

namespace nanofmt {
    struct buffer;

    [[nodiscard]] constexpr char* copy_to(char* buffer, char const* end, char const* source) noexcept;
    [[nodiscard]] constexpr char* copy_to_n(
        char* buffer,
        char const* end,
        char const* source,
        std::size_t length) noexcept;
    [[nodiscard]] constexpr char* put(char* buffer, char const* end, char ch) noexcept;
    [[nodiscard]] constexpr char* fill_n(char* buffer, char const* end, char ch, std::size_t count) noexcept;
} // namespace nanofmt

/// Small wrapper about a character sequence.
///
/// Counts the number of characters that are written to the buffer.
///
/// Use advance_to to update the pos pointer to ensure the advance field
/// is updated appropriately.
///
struct nanofmt::buffer {
    constexpr buffer(char* const dest, std::size_t count) noexcept : pos(dest), end(dest + count) {}

    constexpr buffer& append(char const* const zstr) noexcept {
        char* const p = nanofmt::copy_to(pos, end, zstr);
        std::size_t const consumed = p - pos;
        advance += consumed;
        advance += __builtin_strlen(zstr + consumed);
        pos = p;
        return *this;
    }

    constexpr buffer& append(char const* source, std::size_t length) noexcept {
        advance += length;
        pos = nanofmt::copy_to_n(pos, end, source, length);
        return *this;
    }

    constexpr buffer& append(char ch) noexcept {
        ++advance;
        pos = nanofmt::put(pos, end, ch);
        return *this;
    }

    constexpr buffer& fill_n(char ch, std::size_t count) noexcept {
        advance += count;
        pos = nanofmt::fill_n(pos, end, ch, count);
        return *this;
    }

    constexpr buffer& advance_to(char* const p) noexcept {
        advance += p - pos;
        pos = p;
        return *this;
    }

    char* pos = nullptr;
    char const* end = nullptr;
    std::size_t advance = 0;
};

constexpr char* nanofmt::copy_to(char* buffer, char const* end, char const* source) noexcept {
    char const* ptr = source;
    while (*ptr != 0 && buffer != end)
        *buffer++ = *ptr++;

    return buffer;
}

constexpr char* nanofmt::copy_to_n(char* buffer, char const* end, char const* source, std::size_t length) noexcept {
    char const* source_end = source + length;
    while (source != source_end && buffer != end)
        *buffer++ = *source++;

    return buffer;
}

constexpr char* nanofmt::put(char* buffer, char const* end, char ch) noexcept {
    if (buffer != end) {
        *buffer++ = ch;
    }
    return buffer;
}

constexpr char* nanofmt::fill_n(char* buffer, char const* end, char ch, std::size_t count) noexcept {
    char const pad_buffer[] = {ch, ch, ch, ch, ch, ch, ch, ch};
    constexpr std::size_t pad_length = sizeof pad_buffer;

    while (count >= pad_length) {
        buffer = copy_to_n(buffer, end, pad_buffer, pad_length);
        count -= pad_length;
    }
    return copy_to_n(buffer, end, pad_buffer, count);
}
