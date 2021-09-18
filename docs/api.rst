:github_url: https://github.com/seanmiddleditch/nanofmt/blob/main/docs/api.rst

.. _api:

API
===

.. contents::

.. _format-api:

Formatting
----------

Core Format API
^^^^^^^^^^^^^^^

The format API is available in the header ``nanofmt/format.h``.

Extensions for C++ standard library string types are in the header
``nanofmt/std_string.h``.

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

The buffer API is available in the header ``nanofmt/buffer.h``.

.. doxygenstruct:: nanofmt::buffer

.. doxygenfunction:: copy_to(char*, char const*, char const*) noexcept

.. doxygenfunction:: copy_to_n(char*, char const*, char const*, std::size_t) noexcept

.. doxygenfunction:: put(char*, char const*, char) noexcept

.. doxygenfunction:: fill_n(char*, char const*, char, std::size_t) noexcept

Values
^^^^^^

The format API is available in the header ``nanofmt/format.h``.

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
specializations that work on length-delimited string views, by deriving
from ``formatter<format_string_view>``.

.. doxygenstruct:: nanofmt::format_string

.. doxygenfunction:: nanofmt::to_format_string

.. doxygenstruct:: nanofmt::format_string_view

Variadic Arguments
^^^^^^^^^^^^^^^^^^

.. doxygenstruct:: nanofmt::format_args

.. doxygenfunction:: nanofmt::make_format_args

.. _to-char-api:

Character Conversion
--------------------

Core Conversion API
^^^^^^^^^^^^^^^^^^^

The character conversion API is available in the header ``nanofmt/to_chars.h``.

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, IntegerT, int_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, FloatT, float_format) noexcept

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, FloatT, float_format, int) noexcept

Supporting Types
^^^^^^^^^^^^^^^^

.. doxygenenum:: nanofmt::int_format

.. doxygenenum:: nanofmt::float_format
