:github_url: https://github.com/seanmiddleditch/nanofmt/docs/api.rst

.. _faq:

Frequently Asked Questions
==========================

Q. Why??
--------

Because.

More seriously, the author works on a team (and has worked on numerous teams)
that have strong cultural tendencies to prefering ``snprintf`` for all string
formatting. Reasons cited are usually something like:

- Minimizing dependencies, as a reason not to use fmtlib.
- Supporting older compilers, as a reason not to use ``std::format``.
- Deep distrust of standard headers, to point of outright banning most
  standard headers that can possibly be avoided or reimplementing
  intrinsic wrappers in other cases. Concessions made to "magic" types
  that the compiler treats specially, such as ``std::memcpy`` and the like.
- Dislike of namespaces. This is mostly a complaint about C++ and how
  adding specialization of standard types requires closing and reopening
  namespaces. One project the author works on effectively doesn't use C++
  namespaces, instead preferring C-style prefixes. e.g., ``fooType`` instead
  of ``foo::Type``. Note that nanofmt at this time doesn't "help" here but
  it's a very possible future evolution.
- Compile times. This is the most-cited reason for "game developers"
  though of course it's a complex and nuanced topic with many strongly-held
  conflicting opinions.

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

Q. Won't C++20 Modules Make This Obsolute?
------------------------------------------

Maybe? Hopefully? Less duplicate code to maintain is only a good thing.

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

.. _`formatxx`: https://github.com/seanmiddleditch/formatxx/
.. _`Boost.Format`: https://www.boost.org/doc/libs/1_77_0/libs/format/doc/format.html

Q. Will This be Maintained?
---------------------------

Excellent question.

... too soon to tell. If having a dedicated maintainer is important to you,
this library might be a little too new and untested for your needs.
