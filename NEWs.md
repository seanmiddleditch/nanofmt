nanofmt
=======

Release 0.3 (WIP)
-----------------

### Breaking Changes

- Renamed `format_output` to `format_context`.
- Custom formatter parsing now uses `format_parse_context`.

### Features

### Bug Fixes

### Infrastructure

- Switch from Catch2 to doctest for testing framework.

Release 0.2
-----------

### Features

- Added `NANOFMT_FLOAT` option to disable float support.
- Support implicit conversion to `format_string`.

### Bug Fixes

- Correct `format_length` with integer format args (#54).

Release 0.1
-----------

- Initial release.
