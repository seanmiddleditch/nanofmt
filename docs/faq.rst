:github_url: https://github.com/seanmiddleditch/nanofmt/blob/main/docs/api.rst

FAQ
===

Q. Why??
--------

Because.

More seriously, the author works on a team (and has worked on numerous teams)
that have strong cultural tendencies to prefering ``snprintf`` for all string
formatting. Reasons cited are usually something like:

- Minimizing dependencies, as a reason not to use fmtlib.
- Supporting older compilers, as a reason not to use ``std::format``.
- Deep distrust of standard headers, as a reason to avoid ``std::format``;
  and any library that uses many standard headers, as a reason not to use
  fmtlib.
- Dislike of namespaces. Note that nanofmt at this time doesn't "help" here but
  it's a very possible/probable future evolution.
- Compile times, as a reason to reinvent every wheel.

Q. Compile Times? Really?
-------------------------

That's two questions, but yes and yes.

It is common (though not universal!) in industries like AAA game development
to heavily optimize for compilation times. The primary reason for this is
*iteration speed*.

This is a large, complex, and nuanced topic. It is very inaccurate to say that
"all game developers" need or care about X.

The best summary for this FAQ, though: some developers critically value having
very fast edit-run-test cycles and will spend considerable time and effort
up-front to make those cycles faster later.

nanofmt is meant for teams who include "minimizing C++ compilation times" in
their efforts to achieve the fastest edit-run-test cycles.

Q. Why Avoid Standard Headears?
-------------------------------

Historically, many standard headers have been very "heavy," introducing
tens or hundreds of thousands of lines of complex C++ into any translation
unit including them. Compile times have suffered.

Beyond the headers themselves, standard library implementations are generally
written to a level of completeness and foolproof-ness that imposes costs
some developers don't wish to pay for.

For example, the `checked iterators_` machinery in MSVC and the equivalents
in libstdc++ and libc++ impose a cost. Even when disabled, the cost
is found in numerous small wrapper functions and other utilities that tend
to decrease compiler throughput for an otherwise disabled feature.

Teams that care about these items will often write code that is more like
C, with plenty of raw pointers and thin abstractions, simply because it's
easier and faster for the compiler to process.

Q. How Does nanofmt Help With Compile Times?
--------------------------------------------

To be clear, nanofmt at this point has not been extensively benchmarked with
any rigor, and we're only _assuming_ it helps.

The design around not using :ref:`output iterators<design-output-iterators>`
is one way nanofmt aims to improve over fmtlib or ``std::format`` for teams
that deeply care about compile times.

In general, nanofmt is written to be more C-like in the sense that
abstractions are minimal, plain values are used where possible, and raw
pointers are used exclusively in place RAII containers, smart pointers, or
iterator wrappers.

The result is simply a lot less code to do (some of) the same stuff. The
loss of abstractions necessarily comes with a large loss of functionality
and flexibility. nanofmt is the result when the choice is made to lean in
favor of simpler (and faster to compile) code rather than more featureful
but complicated code.

.. _faq-modules:

Q. Won't C++20 Modules Make This Obsolute?
------------------------------------------

Maybe? Hopefully? Less duplicate code to maintain is only a good thing. I
will only be happy if I never have to personally think about a floating-
point string conversion routine again.

The reality is likely a bit more murky. For one, nanofmt exists *now* but
C++20 modules are possibly still years away from even being a viable option
for new projects. At the time of writing, compiler support is incomplete and
very buggy; build system support is nearly non-existant; module-aware linters
and doc generators and like is also non-existant; and the general user and
ecosystem support can best be described as nascent.

Further, note that modules only help *part* of the compile time overhead.
At best, we can expect modules to reduce the cost of parsing large header
hierarchies. While that is a significant amount of the time incurred with
compiling libraries like fmtlib or ``std::format``, another large chunk of the
time goes into instantiating templates, resolving function overloads,
evaluating constexpr functions, and so on.

nanofmt, by virtue of steeply limiting its feature set and general
applicability, aims to reduce the need for as much of that "use time"
overhead as possible. While it's almost certainly impossible to hit the
minimal compile-time of ``snprintf``, the goal is to keep the difference small
enough that the "developer time" benefits of a type-safe user-extensible
format library outweighs the compile time costs.

Q. Was it Worth It?
-------------------

Probably not.

The nanofmt author implemented a (very bare-bones) fmtlib replacement at
work in about a day, and it's likely going to be incorporated into the core
runtime libraries of the company as-is.

In comparison, the author has been working on nanofmt for 12+ hours/day
for about a week; and that doesn't include all the time the author spent
building the precursors to nanofmt in personal projects, going all the way
back to `formatxx`_ (an "ancient" C++11 library), and including
re-writing formatxx to incorporate into commercial game codebases with
specialized requirements (like drop-in `Boost.Format`_ compatibility).

Q. Will This be Maintained?
---------------------------

Excellent question.

... too soon to tell. If having a dedicated maintainer is important to you,
this library might be a little too new and untested for your needs.

As stated in the :ref:`C++ modules FAQ question<faq-modules>`, there's a very real future where this entire
library is obsolete. To that end, while nanofmt is not a direct drop-in
replacement for ``std::format``, it aims to be "close enough" that migrating
from nanofmt to the standard equivalent is meant to be straightforward.

.. _`formatxx`: https://github.com/seanmiddleditch/formatxx/
.. _`Boost.Format`: https://www.boost.org/doc/libs/1_77_0/libs/format/doc/format.html
.. _`checked iterators`: https://docs.microsoft.com/en-us/cpp/standard-library/checked-iterators
