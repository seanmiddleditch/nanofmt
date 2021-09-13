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

.. doxygenfunction:: nanofmt::format_length

.. doxygenfunction:: nanofmt::vformat_length

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

.. doxygenfunction:: nanofmt::to_chars(char*, char const*, signed int, int_format)
