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
excellent [fmtlib][fmtlib] by Victor Zverovich and contributors. This can be
used in environments or team cultures where neither [`std::format`][stdfmt] nor
fmtlib are available for use.

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

Benchmarks
----------

### Compilation

TBD

### Runtime

TBD

Design
------

### Origins

The design of nanofmt started by trying to closely match the interface of
`std::format` and fmtlib. This includes the general naming of functions
and types as well as the design of the `make_format_args` and related
utilities.


### Output Iterators

A major design note of nanofmt is that the only output target supported
are `char` arrays. This is one of the largest simplifying factors in
nanofmt vs either `std::format` or fmtlib.

This is feasible since fixed-size buffers and the complete avoidance
of any allocating routines is exceedingly common in nanofmt's target
domains. In the rare cases that arbitrary-length formatting is
required, the use of `format_length` allows pre-allocating any
necessary buffer.

For one, arbitrary output iterator support effectively requires all
formatting code to live in headers, with only the occassional pieces
living in individual TUs. This is because the `format_context<>`
in `std::format`/fmtlib is itself a type template.

Supporting arbitrary output iterators requires having full output
iterator machinery in the first place. Output iterators that need to
functions with fixed-length outputs are non-trivial to write.

More importantly, making such generalized output iterator support
reasonably fast requires _even more_ complexity in terms of temporary
buffers, buffer flushing, and so on. Such buffering mechanisms are
also required to enable any kind of efficient type-erasure of the output
iterators.

There's even further machinery necessary for making common output iterators
efficient. Consider `back_inserter<T>`. An efficient implementation of
`std::format` needs to detect that its output iterator is a `back_inserter`
and transparently replace calls to `push_back` to more efficient `append` or
`insert` invocations.

The use of these output iterators is mostly to support things like streaming
to console IO, to support `push_back` into containers like `std::string`, or
esoteric filtering mechanisms. None of these are essential to nanofmt's
target use cases.

The design and implementation pioneered by Victor for fmtlib is some honestly
amazing engineering! Victor and other fmtlib contributors deserve nothing
but praise and respect for the incredible amount of work done to make fmtlib
(and by extension `std::format`) feel natural, intuitive, and unsurprising in
C++ while still having exceptionally good runtime efficiency.

nanofmt however is more for teams that feel that C-like APIs like `snprintf`
are already the epitome of being natural, intuitive, and unsurprising;
except of course for the limitations imposed by C's varargs vs C++'s
variadic tempaltes.

Ultimately, the complexity cost of supporting other kinds of output iterators
is high, and the benefit for nanofmt users is low.

### Localization

nanofmt only supports `char` and does not bother with any of `wchar_t`/
`char8_t`, `char16_t`, or `char32_t`.

Additionally, the `L` format specification flag is parsed but
ignored.

nanofmt only supports `char` because that is, by and large, the only
character type in active use in its target domains. Target systems can and do
assume that all `char*` strings are UTF-8. The type-system bifurcation
created by `char8_t` has caused problems for the few projects that took to
using `u8""` literals, as it required every function taking a `char*` to offer
a second overload that also accepted `char8_t*`. The result has mostly been
projects starting to use `u8` literals in C++17 and abandoning them soon
after since C++20 compatibility changed the types in a very incompatible and
gratuitous way.

The localization flags are unsupported as the target uses of nanofmt tend not
to ever bother with localization. Logging can actually be harmed by
localization as it makes log parsers and alert systems far more difficult to
deploy and maintain. "User-facing" formatting in nanofmt's target domains is
generally just developer tools and utilities, which are effectively never
localized due both to the cost of localizing propriety in-house tools and to
the rapid rate of change in such tools; keeping localization up-to-date is
not especially feasible in those environments.

While nanofmt targets games developers in particular, and games _are_ heavily
and frequently localized, it is not expected that nanofmt would be used for
player-facing text. Game UI text tends to use heavily specialized toolkits
and rely on iconography, layout, color and style, and other factors to convey
information; nanofmt-like text formatting is exceptionally rare in such UI.

### to_chars

nanofmt includes implementations of `to_chars` mostly because there are
shipping "C++17" implementations that are very much in common use in target
industries but which do not offer complete `to_chars` implementations. This
is especially true for `float`/`double`.

nanofmt uses the Dragonbox reference implementation in its floating-point
`to_chars` implementations. There are no fallbacks to other implementations
as found in fmtlib or `std::to_chars`. This in particular limits the
precision of fixed-point formats in nanofmt.

The precision limitation is not currently believed to be a showstopper, but
may be revisited if use cases from nanofmt users illustrates a strong need
for more intricate fixed-point formatting.

### constexpr

Most of nanofmt is not constexpr. This is an intentional choice.

Making a constexpr-friendly formatting library unfortunately requires that
most of the implementation of the formatters and all supporting machinery
also be constexpr, which in turn means it all has to live in headers.

This might be a much smaller issue once we're all living with C++20
modules, but today, the cost paid by every user for constexpr capabilties
is very high.

That said, constexpr formatting is a generally useful feature. nanofmt may,
if use cases arise, offer a second `const_format.h` header or the like
which includes/imports constexpr definitions of the format implementations.
Such an approach would allow individual TUs to opt-in to pulling in all the
machinery if and only if they actually need it.

Note that we have thus far kept the _parsing_ part of nanofmt all constexpr
capable, since we may wish to enable compile-time format string checking
capabilities for projects that (wisely) prefer such a feature. All known
potential users of nanofmt are not yet using C++20 so compile-time checking
isn't a priority. We may instead opt to just entirely drop the constexpr
parsing support if we decide we're not going to support it anytime soon.

### Alignment, Width, Alternate Forms, and Other Specifiers

nanofmt implements are relatively limited set of the fmtlib/`std::format`
specifiers. All are parsed, but most are ignored.

This isn't a "design" so much as just not having had the use cases made
for supporting all of them yet. Alternate form for integers is high on the
list, though.

The goal isn't to be feature-complete, and some of these specifiers are
_juuust_ annoying enough to implement that it'll only be done on-demand.

Frequently Asked Questions
--------------------------

### Q. Why??

Because.

More seriously, the author works on a team (and has worked on numerous teams)
that have strong cultural tendencies to prefering `snprintf` for all string
formatting. Reasons cited are usually something like:

- Minimizing dependencies, as a reason not to use fmtlib.
- Supporting older compilers, as a reason not to use `std::format`.
- Deep distrust of standard headers, to point of outright banning most
  standard headers that can possibly be avoided or reimplementing
  intrinsic wrappers in other cases. Concessions made to "magic" types
  that the compiler treats specially, such as `std::memcpy` and the like.
- Dislike of namespaces. This is mostly a complaint about C++ and how
  adding specialization of standard types requires closing and reopening
  namespaces. One project the author works on effectively doesn't use C++
  namespaces, instead preferring C-style prefixes. e.g., `fooType` instead
  of `foo::Type`. Note that nanofmt at this time doesn't "help" here but
  it's a very possible future evolution.
- Compile times. This is the most-cited reason for "game developers"
  though of course it's a complex and nuanced topic with many strongly-held
  conflicting opinions.

### Q. Compile Times? Really?

That's two questions, but yes and yes.

It is common (though not universal!) in industries like AAA game development
to heavily optimize for compilation times. The primary reason for this is
_iteration speed_.

This is a large, complex, and nuanced topic. It is very inaccurate to say that
"all game developers" need or care about X.

The best summary for this FAQ, though: some developers critically value having
very fast edit-run-test cycles and will spend considerable time and effort
up-front to make those cycles faster later.

nanofmt is meant for teams who include "minimizing C++ compilation times" in
their efforts to achieve the fastest edit-run-test cycles.

### Q. Won't C++20 Modules Make This Obsolute?

Maybe? Hopefully? Less duplicate code to maintain is only a good thing.

The reality is likely a bit more murky. For one, nanofmt exists _now_ but
C++20 modules are possibly still years away from even being a viable option
for new projects. At the time of writing, compiler support is incomplete and
very buggy; build system support is nearly non-existant; module-aware linters
and doc generators and like is also non-existant; and the general user and
ecosystem support can best be described as nascent.

Further, note that modules only help _part_ of the compile time overhead.
At best, we can expect modules to reduce the cost of parsing large header
hierarchies. While that is a significant amount of the time incurred with
compiling libraries like fmtlib or `std::format`, another large chunk of the
time goes into instantiating templates, resolving function overloads,
evaluating constexpr functions, and so on.

nanofmt, by virtue of steeply limiting its feature set and general
applicability, aims to reduce the need for as much of that "use time"
overhead as possible. While it's almost certainly impossible to hit the
minimal compile-time of `snprintf`, the goal is to keep the difference small
enough that the "developer time" benefits of a type-safe user-extensible
format library outweighs the compile time costs.

### Q. Was it Worth It?

Probably not.

The nanofmt author implemented a (very bare-bones) fmtlib replacement at
work in about a day, and it's likely going to be incorporated into the core
runtime libraries of the company as-is.

In comparison, the author has been working on nanofmt for 12+ hours/day
for about a week; and that doesn't include all the time the author spent
building the precursors to nanofmt in personal projects, going all the way
back to [formatxx][fmtxx] (an "ancient" C++11 library), and including
re-writing formatxx to incorporate into commercial game codebases with
specialized requirements (like drop-in [Boost.Format][boofmt] compatibility).

[fmtxx]: https://github.com/seanmiddleditch/formatxx/
[boofmt]: https://www.boost.org/doc/libs/1_77_0/libs/format/doc/format.html

### Q. Will This be Maintained?

Excellent question.

... too soon to tell. If having a dedicated maintainer is important to you,
this library might be a little too new and untested for your needs.
