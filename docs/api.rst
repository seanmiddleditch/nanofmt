:github_url: https://github.com/seanmiddleditch/nanofmt/blob/main/docs/api.rst

.. _api:

API
===

.. contents::

.. _format-api:

Formatting
----------

The format API is available in the header ``nanofmt/format.h``.

Extensions for C++ standard library string types are in the header
``nanofmt/std_string.h``.

Core API
^^^^^^^^

.. doxygenfunction:: nanofmt::format_to(char (&dest)[N], format_string, Args const&...)

.. doxygenfunction:: nanofmt::format_to(buffer&, format_string, Args const&...)

.. doxygenfunction:: nanofmt::vformat_to(char (&dest)[N], format_string, format_args&&)

.. doxygenfunction:: nanofmt::vformat_to(buffer&, format_string, format_args&&)

.. doxygenfunction:: nanofmt::format_to_n(char*, std::size_t, format_string, Args const&...)

.. doxygenfunction:: nanofmt::vformat_to_n(char*, std::size_t, format_string, format_args&&)

.. doxygenfunction:: nanofmt::format_size

.. doxygenfunction:: nanofmt::vformat_size

.. doxygenfunction:: nanofmt::make_format_args

Buffers
^^^^^^^

.. doxygenstruct:: nanofmt::buffer

Values
^^^^^^

Individual values can be formatted with nanofmt without requiring a full
format string.

.. doxygenfunction:: nanofmt::format_value_to(char (&dest)[N], ValueT const&, format_string)

.. doxygenfunction:: nanofmt::format_value_to(buffer&, ValueT const&, format_string)

.. doxygenfunction:: nanofmt::format_value_to_n(char*, std::size_t, ValueT const&, format_string)

.. doxygenfunction:: nanofmt::format_value_size(ValueT const&, format_string)

Miscellaneous
^^^^^^^^^^^^^

nanofmt uses a ``format_string`` type for receiving its format strings, to
decouple from and support various string types and classes. Many string
types should automatically convert to ``format_string``; for string types
that don't already support conversion to ``format_string``, a
``to_format_string`` function can be implemented.

A very simple ``format_string_view`` that wraps a ``char const*`` and a
``std::size_t`` is provided to make it easier to write ``formatter``
specializations that work on length-delimited string views.

.. doxygenstruct:: nanofmt::format_string

.. doxygenfunction:: nanofmt::to_format_string

.. doxygenstruct:: nanofmt::format_string_view

Syntax Parsers
^^^^^^^^^^^^^^

.. doxygenstruct:: nanofmt::format_spec

.. doxygenfunction:: nanofmt::parse_spec

Variadic Arguments
^^^^^^^^^^^^^^^^^^

.. doxygenstruct:: nanofmt::format_args

.. doxygenfunction:: nanofmt::make_format_args

.. _to-char-api:

Character Conversion
--------------------

The character conversion API is available in the header ``nanofmt/to_chars.h``.

Integral Types
^^^^^^^^^^^^^^

.. doxygenenum:: nanofmt::int_format

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed char, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed short, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed int, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed long, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed long long, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned char, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned short, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned int, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned long, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned long long, int_format) noexcept

Floating Point Types
^^^^^^^^^^^^^^^^^^^^

.. doxygenenum:: nanofmt::float_format

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, float, float_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, double, float_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, float, float_format, int) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, double, float_format, int) noexcept
