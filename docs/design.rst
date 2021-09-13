:github_url: https://github.com/seanmiddleditch/nanofmt/blob/main/docs/design.rst

Design
======

Origins
-------

The design of nanofmt started by trying to closely match the interface of
``std::format`` and fmtlib. This includes the general naming of functions
and types as well as the design of the ``make_format_args`` and related
utilities.

.. _design-output-iterators:

Output Iterators
----------------

A major design note of nanofmt is that the only output target supported
are ``char`` arrays. This is one of the largest simplifying factors in
nanofmt vs either ``std::format`` or fmtlib.

This is feasible since fixed-size buffers and the complete avoidance
of any allocating routines is exceedingly common in nanofmt's target
domains. In the rare cases that arbitrary-length formatting is
required, the use of ``format_length`` allows pre-allocating any
necessary buffer.

For one, arbitrary output iterator support effectively requires all
formatting code to live in headers, with only the occassional pieces
living in individual TUs. This is because the ``format_context<>``
in ``std::format``/fmtlib is itself a type template.

Supporting arbitrary output iterators requires having full output
iterator machinery in the first place. Output iterators that need to
functions with fixed-length outputs are non-trivial to write.

More importantly, making such generalized output iterator support
reasonably fast requires *even more* complexity in terms of temporary
buffers, buffer flushing, and so on. Such buffering mechanisms are
also required to enable any kind of efficient type-erasure of the output
iterators.

There's even further machinery necessary for making common output iterators
efficient. Consider ``back_inserter<T>``. An efficient implementation of
``std::format`` needs to detect that its output iterator is a ``back_inserter``
and transparently replace calls to ``push_back`` to more efficient ``append`` or
``insert`` invocations.

The use of these output iterators is mostly to support things like streaming
to console IO, to support ``push_back`` into containers like ``std::string``, or
esoteric filtering mechanisms. None of these are essential to nanofmt's
target use cases.

The design and implementation pioneered by Victor Zverovich for fmtlib is some
honestly amazing engineering! Victor and other fmtlib contributors deserve
nothing but praise and respect for the incredible amount of work done to make
fmtlib (and by extension ``std::format``) feel natural, intuitive, and
unsurprising in C++ while still having exceptionally good runtime efficiency.

nanofmt however is more for teams that feel that C-like APIs like ``snprintf``
are already the epitome of being natural, intuitive, and unsurprising;
except of course for the limitations imposed by C's varargs vs C++'s
variadic tempaltes.

Ultimately, the complexity cost of supporting other kinds of output iterators
is high, and the benefit for nanofmt users is low.

Localization
------------

nanofmt only supports ``char`` and does not bother with any of ``wchar_t``/
``char8_t``, ``char16_t``, or ``char32_t``.

Additionally, the ``L`` format specification flag is parsed but
ignored.

nanofmt only supports ``char`` because that is, by and large, the only
character type in active use in its target domains. Target systems can and do
assume that all ``char*`` strings are UTF-8. The type-system bifurcation
created by ``char8_t`` has caused problems for the few projects that took to
using ``u8""`` literals, as it required every function taking a ``char*`` to offer
a second overload that also accepted ``char8_t*``. The result has mostly been
projects starting to use ``u8`` literals in C++17 and abandoning them soon
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

While nanofmt targets games developers in particular, and games *are* heavily
and frequently localized, it is not expected that nanofmt would be used for
player-facing text. Game UI text tends to use heavily specialized toolkits
and rely on iconography, layout, color and style, and other factors to convey
information; nanofmt-like text formatting is exceptionally rare in such UI.

.. _design-char-conv:

Character Conversion
--------------------

nanofmt includes implementations of ``to_chars`` mostly because there are
shipping "C++17" implementations that are very much in common use in target
industries but which do not offer complete ``to_chars`` implementations. This
is especially true for ``float``/``double``.

nanofmt uses the Dragonbox reference implementation in its floating-point
``to_chars`` implementations. There are no fallbacks to other implementations
as found in fmtlib or ``std::to_chars``. This in particular limits the
precision of fixed-point formats in nanofmt.

The precision limitation is not currently believed to be a showstopper, but
may be revisited if use cases from nanofmt users illustrates a strong need
for more intricate fixed-point formatting.

constexpr
---------

Most of nanofmt is not constexpr. This is an intentional choice.

Making a constexpr-friendly formatting library unfortunately requires that
most of the implementation of the formatters and all supporting machinery
also be constexpr, which in turn means it all has to live in headers.

This might be a much smaller issue once we're all living with C++20
modules, but today, the cost paid by every user for constexpr capabilties
is very high.

That said, constexpr formatting is a generally useful feature. nanofmt may,
if use cases arise, offer a second ``const_format.h`` header or the like
which includes/imports constexpr definitions of the format implementations.
Such an approach would allow individual TUs to opt-in to pulling in all the
machinery if and only if they actually need it.

Note that we have thus far kept the parsing* part of nanofmt all constexpr
capable, since we may wish to enable compile-time format string checking
capabilities for projects that (wisely) prefer such a feature. All known
potential users of nanofmt are not yet using C++20 so compile-time checking
isn't a priority. We may instead opt to just entirely drop the constexpr
parsing support if we decide we're not going to support it anytime soon.

Alignment, Width, Alternate Forms, and Other Specifiers
-------------------------------------------------------

nanofmt implements are relatively limited set of the fmtlib/``std::format``
specifiers. All are parsed, but most are ignored.

This isn't a "design" so much as just not having had the use cases made
for supporting all of them yet. Alternate form for integers is high on the
list, though.

The goal isn't to be feature-complete, and some of these specifiers are
*juuust* annoying enough to implement that it'll only be done on-demand.
