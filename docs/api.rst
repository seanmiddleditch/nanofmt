:github_url: https://github.com/seanmiddleditch/nanofmt/blob/main/docs/api.rst

.. _api:

API
===

.. contents::

.. _format-api:

Formatting
----------

Core API
^^^^^^^^

.. doxygenfunction:: nanofmt::format_to(char (&)[N], FormatT&&, Args const&...)

.. doxygenfunction:: nanofmt::format_to(buffer&, FormatT&&, Args const&...)

.. doxygenfunction:: nanofmt::vformat_to(char (&)[N], FormatT&&, format_args&&)

.. doxygenfunction:: nanofmt::vformat_to(buffer&, FormatT&&, format_args&&)

.. doxygenfunction:: nanofmt::format_to_n(char*, std::size_t, FormatT&&, Args const&...)

.. doxygenfunction:: nanofmt::vformat_to_n(char*, std::size_t, FormatT&&, format_args&&)

.. doxygenfunction:: nanofmt::format_size

.. doxygenfunction:: nanofmt::vformat_size

.. doxygenfunction:: nanofmt::make_format_args

Buffers
^^^^^^^

.. doxygenstruct:: nanofmt::buffer

Strings
^^^^^^^

.. doxygenstruct:: nanofmt::string_view

.. doxygenfunction:: constexpr nanofmt::to_string_view(StringT const&) noexcept

.. doxygenfunction:: nanofmt::to_string_view(char const (&)[N]) noexcept

.. doxygenfunction:: nanofmt::to_string_view(char const *) noexcept

.. doxygenfunction:: nanofmt::to_string_view(string_view) noexcept


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
