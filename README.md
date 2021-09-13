nanofmt
=======

https://github.com/potatoengine/nanofmt

License
-------

Copyright (c) Sean Middleditch and contributors

nanofmt is released under the [MIT license][mitlic].

nanofmt uses the [Dragonbox][drgbox] reference implementation which released
under either the [Apache License Version 2.0 with LLVM Exceptions][apclic] or
the [Boost Software License Version 1.0][boolic].

[mitlic]: https://mit-license.org/
[drgbox]: https://github.com/jk-jeon/dragonbox/
[apclic]: https://raw.githubusercontent.com/jk-jeon/dragonbox/master/LICENSE-Apache2-LLVM
[boolic]: https://raw.githubusercontent.com/jk-jeon/dragonbox/master/LICENSE-Boost

Exmaple
-------

```c++
char buffer[128];

char const* const end = nanofmt::format_to_n(
        buffer,
        sizeof buffer,
        "Hello, {0}! You are visitor {1}.",
        user_name,
        visitor_count);

// void write_log(char const* str, size_t length);
write_log(buffer, end - buffer);
```

About
-----

nanofmt aims to provide a lite-weight semi-compatible implementation of the
excellent [fmtlib][fmtlib]. This can be used in environments or team cultures
where neither [`std::format`][stdfmt] nor fmtlib are available for use.

The primary motivation for nanofmt is to minimize dependencies on standard C++
headers and to minimize compile time. nanofmt does _not_ aim to offer the
fastest runtime efficiency, the most features, fmtlib or `std::format`
compatibility, nor the most portable implementation.

C++17 is required.

Most notably, nanofmt _only_ supports the `(v)format_to_n` interfaces to basic
`char*` buffers and does not support any other character type or output
iterators. There are custom `(v)format_to` wrappers that work only with the
provided `buffer` type, which itself is just a wrapper around `char*` buffers.

nanofmt does _not_ aim to be a true drop-in replacement of fmtlib, even for
the interfaces found in both libraries. Some interfaces have been modified
to appeal more to "C with Classes" programmers; this is not a judgement of
modern C++, just an acquiescence to the prevalent tastes and opinions in
the industries targetted by nanofmt (primarily the AAA game industry).

[fmtlib]: https://github.com/fmtlib/fmt
[stdfmt]: https://en.cppreference.com/w/cpp/utility/format/format

When to Use nanofmt
-------------------

If you're in doubt, use fmtlib! It is in almost every conceivable way more
complete, more runtime efficient, more reliable, better maintained, more
portable, and otherwise superior.

Use nanofmt if your organization or team has decided they won't use fmtlib
because of its standard library dependencies or its large-ish header sizes.

Use nanofmt if your organization or team believes that `snprintf` is the
superior formatting library, other than limitations created by C varargs.

When NOT to Use nanofmt
-----------------------

Literally anytime that you have the option of using `std::format` or fmtlib,
you should prefer using those instead!

If you're not sure if nanofmt is the right fit for your project, then
it isn't. :)
