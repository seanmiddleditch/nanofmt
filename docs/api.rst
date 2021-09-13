.. _api:

API Reference
=============

.. contents::

.. _format-api:

Utilities
=========

.. doxygenstruct:: nanofmt::string_view

.. doxygenfunction:: nanofmt::to_string_view(StringT const&)

Formatting
==========

.. doxygenfunction:: nanofmt::format_to(char (&)[N], FormatT&&, Args const&...)

.. doxygenfunction:: nanofmt::vformat_to(char (&)[N], FormatT&&, format_args&&)

.. doxygenfunction:: nanofmt::format_to_n(char*, std::size_t, FormatT&&, Args const&...)

.. doxygenfunction:: nanofmt::vformat_to_n(char*, std::size_t, FormatT&&, format_args&&)

.. doxygenfunction:: nanofmt::format_size

.. doxygenfunction:: nanofmt::vformat_size

.. doxygenfunction:: nanofmt::make_format_args

Syntax Parsers
--------------

.. doxygenstruct:: nanofmt::format_spec

Variadic Arguments
------------------

.. doxygenstruct:: nanofmt::format_args

.. doxygenfunction:: nanofmt::make_format_args

.. _to-char-api:

Character Conversion
====================

Enumerations
------------

.. doxygenenum:: nanofmt::int_format

.. doxygenenum:: nanofmt::float_format

Signed Integers
---------------

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed char, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed short, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed int, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed long, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, signed long long, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned char, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned short, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned int, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned long, int_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, unsigned long long, int_format)

Floating Point
------------------

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, float, float_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, double, float_format)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, float, float_format, int)

.. doxygenfunction:: nanofmt::to_chars(char *, char const *, double, float_format, int)
