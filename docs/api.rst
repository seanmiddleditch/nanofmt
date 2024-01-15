:github_url: https://github.com/seanmiddleditch/nanofmt/blob/main/docs/api.rst

.. _api:

API
===

.. contents::

.. _format-api:

Formatting
----------

The format API is available in the header ``nanofmt/format.h``.

The header ``nanofmt/forward.h`` offers forward declarations of
nanofmt types, including the ``formatter<T>`` template that users
must specialize to support custom types.

Extensions for C++ standard library string types are in the header
``nanofmt/std_string.h``.

Format to Array
^^^^^^^^^^^^^^^

The :cpp:func:`nanofmt::format_to` functions format a given format string
and arguments into the target buffer. The result will be NUL-terminated.
The return value is a pointer to the terminating NUL character.

The :cpp:func:`nanofmt::format_append_to` functions format a given format
string and arguments onto the end of target buffer. The result will be NUL-
terminated. The return value is a pointer to the terminating NUL character.

.. cpp:function:: char* nanofmt::format_to(char (&dest)[N], format_string format_str, Args const&... args)

.. cpp:function:: char* nanofmt::vformat_to(char (&dest)[N], format_string format_str, format_args args)

.. cpp:function:: char* nanofmt::format_append_to(char (&dest)[N], format_string format_str, Args const&... args)

.. cpp:function:: char* nanofmt::vformat_append_to(char (&dest)[N], format_string format_str, format_args args)

Length-Delimited Formatting
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :cpp:func:`nanofmt::format_to_n` functions format a given format string
and arguments into the target buffer, up to the given number of characters.
The result will **NOT** be NUL-terminated. The return value is a pointer to
one past the last character written.

The :cpp:func:`nanofmt::format_append_to_n` functions format a given format
string and arguments onto the end of the target buffer, up to the given
number of characters. The result will **NOT** be NUL-terminated. The return
value is a pointer to one past the last character written.

.. cpp:function:: char* nanofmt::format_to_n(char* dest, std::size_t count, format_string format_str, Args const&... args)

.. cpp:function:: char* nanofmt::vformat_to_n(char* dest, std::size_t count, format_string format_str, format_args&&)

.. cpp:function:: char* nanofmt::format_append_to_n(char* dest, std::size_t count, format_string format_str, Args const&... args)

.. cpp:function:: char* nanofmt::vformat_append_to_n(char* dest, std::size_t count, format_string format_str, format_args&&)

Custom Formatters
^^^^^^^^^^^^^^^^^

The :cpp:struct:`nanofmt::formatter<T>` template must be specialized to add
support for user-provided types.

Two member functions, ``parse`` and ``format``, must be implemented on the
specialized structure for nanofmt to work.

.. cpp:struct:: template<typename T> nanofmt::formatter

  Custom formatter. May include any member variables necesasry to convey
  format information from ``parse`` to ``format``.

  .. cpp:function:: char const* parse(format_parse_context& ctx)

    Consumes characters from ``ctx`` up to, but not including, ``ctx.end()``.
    Returns a pointer to one past the last character consumed.

  .. cpp:function:: void format(T const& value, format_context& ctx) const

    Formats ``value`` to ``ctx.out()``.

The parse function takes a type :cpp:struct:`nanofmt::format_parse_context`
that contains information about the current argument and specification string.

.. cpp:struct:: nanofmt::format_parse_context

  Parse context for custom formatters.

  .. cpp:function:: char const* begin() const noexcept

  .. cpp:function:: char const* end() const noexcept

  .. cpp:function:: void advance_to(const char* to) noexcept

A header implementing a custom formatter may choose to only depend on
``nanofmt/foward.h`` header. This header does not offer any of the
implementations, nor does it provide declarations of the formatting
functions. A formatter may work around this by specifying the
``format_context&`` parameter of ``format`` as a template, as in:

.. code-block:: c++

  #include <nanofmt/forward.h>

  namespace nanofmt {
    template<>
    struct formatter<my_type> {
      template <typename ContextT>
      constexpr char const* parse(ContextT& ctx) noexcept;

      template <typename ContextT>
      void format(my_type const& value, ContextT& ctx);
    }
  }

Format Length
^^^^^^^^^^^^^

The :cpp:func:`nanofmt::format_length` function returns the length of result
of formatting the given format string and arguments, excluding any
terminating NUL character.

.. cpp:function:: size_t nanofmt::format_length(format_string format_str, Args const&... args)

.. cpp:function:: size_t nanofmt::vformat_length(format_string format_str, format_args args)

Output Buffers
^^^^^^^^^^^^^^

.. cpp:struct:: nanofmt::format_context

Format to Buffer
^^^^^^^^^^^^^^^^

The ``nanofmt::format_context&`` overloads of :cpp:func:`nanofmt::format_to`
format a given format string and arguments into the target buffer. The result
will **not** be NUL-terminated. The return value is the buffer object itself.

  .. cpp:function:: format_context& format(format_string fmt, Args const&... args)

    Formats the given format string and argument into the buffer.

  .. cpp:function:: format_context& vformat(format_string fmt, format_args args)

    Formats the given format string and argument into the buffer.
    
  .. cpp:function:: constexpr format_context& append(char const* const zstr) noexcept

    Appends the contents of ``zstr`` to the buffer.

  .. cpp:function:: constexpr format_context& append(char const* source, std::size_t length) noexcept

    Appends ``length`` characters from ``source`` to the buffer.

  .. cpp:function:: constexpr format_context& put(char ch) noexcept

    Appends the character ``ch`` to the buffer.

  .. cpp:function:: constexpr format_context& fill_n(char ch, std::size_t count) noexcept

    Appends ``count`` copies of the character ``ch`` to the buffer.

  .. cpp:function:: constexpr char* out() const noexcept

    Returns the current output location within the destination buffer.

  .. cpp:function:: constexpr format_context& advance_to(char* const p) noexcept

    Updates the buffer position to ``p`` and adjusts the ``advance`` member appropriately.

  .. cpp:member:: char* pos = nullptr

    Current output position of the buffer. For custom formatting operations,
    use this value for the output position. The :cpp:func:`advance_to`
    function should always be preferred for mutating the ``pos`` member.

  .. cpp:member:: char const* end = nullptr

    The end pointer for the buffer. Custom formatting code should never
    advance ``pos`` past the ``end`` pointer, and should never dereference
    ``end``.

  .. cpp:member:: std::size_t advance = 0

    The number of characters that were written to the buffer, ignoring any
    truncation. Even when ``pos`` equals ``end``, operations on the buffer
    will still increment ``advance``.

    The :cpp:func:`advance_to` member function should be preferred over
    directly mutating ``advance``.

String Utilities
^^^^^^^^^^^^^^^^

General string utiltities that are useful in implementing formatting.

.. cpp:function:: char* copy_to(char* dest, char const* end, char const* source) noexcept

  Copy the source string to the destination buffer, but not extending past
  the provided buffer end pointer. Returns the pointer one past the last
  character written. 

.. cpp:function:: char* copy_to_n(char* dest, char const* end, char const* source, std::size_t length) noexcept

  Copies up to ``length`` characters of source string to the destination
  buffer, but not extending past the provided buffer end pointer. Returns
  the pointer past the last character written.

.. cpp:function:: char* put(char* dest, char const* end, char ch) noexcept

  Copies the provided character ``ch`` to the destination buffer, but not
  extending past the provided buffer end pointer. Returns the pointer past
  the last character written.

.. cpp:function:: char* fill_n(char* dest, char const* end, char ch, std::size_t count) noexcept

  Copies ``count`` copies of the charcter ``ch`` to the destination buffer,
  but not extending past the provided buffer end pointer. Returns the
  pointer past the last character written. 

.. cpp:function:: std::size_t strnlen(char const* buffer, std::size_t count) noexcept

  Returns the length of the string in buffer, to a maximum of count.

Format Strings
^^^^^^^^^^^^^^

nanofmt uses a :cpp:struct:`nanofmt::format_string` structure for receiving
its format strings, to decouple from and support various string types and
classes. Many string types should automatically convert to ``format_string``;
for string types that don't already support conversion to ``format_string``,
a :cpp:func:`nanofmt::to_format_string` function can be implemented.

.. cpp:struct:: nanofmt::format_string

  Receiver for format strings. Only implicitly constructs from string
  literals (constant character arrays). Can be explicitly constructed
  from other string types.

.. cpp:function:: template <typename T> format_string nanofmt::to_format_string(T const& value) noexcept

  Converts a given string type to a :cpp:struct:`nanofmt::format_string`.
  Works on most standard string types with ``data()`` and ``size()``
  member functions.

  Overload to add support for other string types that require different
  means of converted to a ``format_string``.

.. cpp:struct:: nanofmt::format_string_view

  A very simple wrapper around a pointer and length. This is provided to make
  it easier to write :cpp:struct:`nanofmt::formatter<T>` specializations that
  work on length-delimited string views, by deriving from
  ``nanofmt::formatter<format_string_view>``.

  .. cpp:member:: char const* string = nullptr

  .. cpp:member:: std::size_t length = 0

Variadic Arguments
^^^^^^^^^^^^^^^^^^

.. cpp:struct:: nanofmt::format_args

  List of format args. Typically only constructed from
  :cpp:func:`nanofmt::make_format_args`. Does not take ownership of any
  internal data. 

  .. warning:: Storing an instance of ``format_args`` can result
    in dangling references.

.. cpp:function:: auto nanofmt::make_format_args(Args const&... args)

  .. danger:: This function should usually only be used directly in
    a call to a function accepting a :cpp:struct:`nanofmt::format_args`
    parameter.

.. _to-char-api:

Character Conversion
--------------------

The character conversion API is available in the header ``nanofmt/charconv.h``.

.. cpp:function:: char* nanofmt::to_chars(char* dest, char const* end, IntegerT value, int_format fmt = int_format::decimal) noexcept

  Formats ``value`` into the buffer using the base specified in ``fmt``.

.. cpp:function:: char* nanofmt::to_chars(char* dest, char const* end, FloatT value, float_format fmt) noexcept

  Formats ``value`` into the buffer using the base specified in ``fmt``. Uses
  the shortest precision.

.. cpp:function:: char* nanofmt::to_chars(char* dest, char const* end, FloatT value, float_format fmt, int precision) noexcept

  Formats ``value`` into the buffer using the base specified in ``fmt``. Uses
  the given ``precision``, whose meaning depends on the specified format.

.. cpp:enum-class:: nanofmt::int_format

  Specify whether to use base 10, base 16, or base 2. Base 16 has an uppercase
  variant.

  .. cpp:enumerator:: decimal

    Base 10.

  .. cpp:enumerator:: hex

    Base 16.

  .. cpp:enumerator:: hex_upper
  .. cpp:enumerator:: binary

    Base 2.

.. cpp:enum-class:: nanofmt::float_format

  Specify whether to use scientific, fixed, or general precision formatting.
  Scientific and general also have uppercase variants.

  .. cpp:enumerator:: scientific
  
    Scientific notation formats floating point values as ``[-]d.de[+-]dd``.

  .. cpp:enumerator:: scientific_upper

  .. cpp:enumerator:: fixed

    Fixed-point notation formats floating point values as ``[-]d.dddd``.

  .. cpp:enumerator:: general

    General precision notation formats in either scientific or fixed-point
    notation, depending on the exponent of the value.

  .. cpp:enumerator:: general_upper
