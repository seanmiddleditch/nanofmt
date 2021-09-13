nanofmt
=======

**nanofmt** aims to provide a lite-weight semi-compatible implementation of the
excellent `fmtlib`_ by Victor Zverovich and contributors. This can be
used in environments or team cultures where neither `std::format`_ nor
fmtlib are available for use.

.. contents::

.. toctree::
  :maxdepth: 2
  :caption: Contents:

  self
  api

.. _basic-example:

Basic Usage
-----------

Example usage of writing a string with two variables formatted into the output.

.. code-block:: c++

  char buffer[128];
  nanofmt::format_to(buffer, "Hello, {0}! You are visitor {1}.",
    UserName,
    VisitorCount);

See also our :ref:`detailed-examples`.

.. _api-overview:

API Overview
============

The crux of the API is writing to a ``char`` arrays. Writing directly to an array
just works:

.. code-block:: c++

  char buffer[128];
  format_to(buffer, "Format this {}", 128);

A pointer to a buffer and a length can also be provided for safe writing.

.. code-block:: c++

  format_to_n(buffer, size, "Format this {}", 128);

The format functions all return a ``char*`` pointing to the terminating NUL that
is always written to the buffer.

.. code-block:: c++

  char* end = format_to(buffer, "{} is {}", seven, 7);
  size_t const size = end - buffer;

The nanofmt functions understand the same positional arguments and most format
flags of `std::format`_.

.. code-block:: c++

  format_to(buffer, "{1} then {0}", "Second", "First");
  // buffer: First then Second

If the length of formatted text is required, e.g. for allocating buffer space,
the ``format_length`` function can be used:

.. code-block:: c++

  size_t const length = format_length("{} plus {}", 7, 7);

  char* buffer = (char*)malloc(length + 1/*NUL byte*/);
  format_to_n(buffer, length + 1, "{} plus {}", 7, 7);

nanofmt also includes implementation of the C++17 standard `to_chars`_
functions, for codebases that are unable or unwilling to use the standard
versions, or who are using older compilers that lack support for
floating-point ``to_chars``

See the :ref:`api` for more in-depth coverage of the nanofmt facilities.

The Case for nanofmt
====================

nanofmt may be a good fit for teams or codebases which are unable or unwilling
to use `fmtlib`_ or `std::format`_, particularly if the reasons involve
compilation time or standard library header dependencies.

The only headers nanofmt relies on are ``<type_traits>``, ``<cstddef>``,
``<cstdint>``, and ``<cmath>``.

Teams that might otherwise continue to prefer using ``snprintf`` may find
that nanofmt is far more to their tastes.

Anyone unsure of whether they should use nanofmt as their should almost
certainly consider using fmtlib or ``std::format`` instead.

Both fmtlib and the standard formatting facilities offer far more features,
far more idiomatic C++ support, and integrate far better with both the
rest of the C++ ecosystem and core IO.

.. _detailed-examples:

Detailed Examples
=================

Custom Types
------------

Provide a specialization of `nanofmt::formatter` to enable nanofmt to consume
values of a custom type.

.. code-block:: c++

  struct MyType {
    std::string FirstName;
    std::string LastName;
  };

  namespace nanofmt {
    template <>
    struct formatter<MyType> {
      bool reverse = false;

      constexpr char const* parse(char const* in, char const* end) noexcept;
      inline void format(MyType const& value, buffer& output) noexcept;
    };
  }

  char buffer[128];
  format_into(buffer, "Greetings {:r}!", MyType{"Bob", "Bobson");

  // buffer would contain:
  // Greetings Bobson, Bob

What does that ``:r`` do? That's a custom formatter flag supported by the
``MyType`` formatter. A possible implementation:

.. code-block:: c++

  constexpr char const* nanofmt::formatter<MyType>::parse(char const* in, char const* end) noexcept {
    if (in != end && *in == 'r')
      ++in;
      reverse = true;
    }
    return reverse;
  }

  void nanofmt::formatter<MyType>::format(MyType const& value, buffer& output) noexcept {
    if (reverse)
      format_into(output, "{}, {}", LastName, FirstName);
    else
      format_into(output, "{} {}", FirstName, LastName);
  }

Length-Delimited Buffers
------------------------

nanofmt automatically length-delimits any attempt to write to a `char[]`
array. When using raw pointers, or to futher constrain the output length,
the `format_into_n` functions may be used.

The format functions return a pointer to the last character written (excluding
the NUL byte). This can be used to chain format calls, or to calculate the
written length.

.. code-block:: c++

  char* ptr = GetBufferData();
  size_t size = GetBufferSize();

  char* end = format_into_n(ptr, size, "Format this! {}", value);

  size_t const length = end - ptr;


License
=======

Copyright © Sean Middleditch and contributors

nanofmt is released under the `MIT license`_.

nanofmt uses the `Dragonbox`_ reference implementation by Junekey Jeon which
is released under either the `Apache License Version 2.0 with LLVM Exceptions`_ or
the `Boost Software License Version 1.0`_.

.. _std::format: https://en.cppreference.com/w/cpp/utility/format/format
.. _fmtlib: https://github.com/fmtlib/fmt
.. _MIT license: https://mit-license.org/
.. _Dragonbox: https://github.com/jk-jeon/dragonbox/
.. _Apache License Version 2.0 with LLVM Exceptions: https://raw.githubusercontent.com/jk-jeon/dragonbox/master/LICENSE-Apache2-LLVM
.. _Boost Software License Version 1.0: https://raw.githubusercontent.com/jk-jeon/dragonbox/master/LICENSE-Boost
.. _to_chars: https://en.cppreference.com/w/cpp/utility/to_chars
