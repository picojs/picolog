picolog
========

*picolog* is a minimal, yet flexible logging framework written in C99.

Features:
--------

- Written in pure C99, but compatible with C++
- Only two files (header/source) for easy integration into any build system
- Tiny memory and code footprint
- Simple and minimalistic API
- Flexible and extensible appender handling
- Ability to set logging level (TRACE, DEBUG, INFO, WARN, ERROR, and FATAL)
- Ability to toggle date/time, level, filename/line, and function reporting
- MIT licensed

API:
--------

#### void `my_appender`(const char* `p_entry`, void* `p_user_data`);

Signature for appender function. An appender could write to the console, a file, over a network or any other form of output stream.

#### PLOG_TRACE(fmt, args...)

Writes a TRACE level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### PLOG_DEBUG(fmt, args...)

Writes a DEBUG level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### PLOG_INFO(fmt, args...)

Writes a INFO( level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### PLOG_WARN(fmt, args...)

Writes a WARN level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### PLOG_ERROR(fmt, args...)

Writes a ERROR level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### PLOG_FATAL(fmt, args...)

Writes a FATAL level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### plog_appender_register(p_appender, p_user_data, p_id)

Registers (adds appender to logger) and enables the specified appender.

- `p_appender`  - Pointer to the appender function to register
- `p_user_data` - A pointer supplied to the appender function when writing a log
                  entry. This pointer is not modified by the logger. If not
                  required, pass in NULL for this parameter
- `p_id`        - A pointer to the appender identifier (set by function). If not
                  required, pass in NULL for this parameter

#### plog_appender_unregister(id)

Unregisters appender (removes the appender from the logger).

- `id` - The appender to unreqister

#### plog_appender_enable(id)

Enables the specified appender. NOTE: Appenders are enabled by default after
registration.

- `id` - The appender to enable

#### plog_appender_disable(id)

Disables the specified appender.

- `id` - The appender to disable

#### plog_enable()

Enables logging. NOTE: Logging is enabled by default.

#### plog_disable()

Disables logging.

#### plog_set_level(level)

Sets the logging level. Only those messages of equal or higher priority
(severity) than this value will be logged.

- `level` - The new global logging threshold.

#### plog_turn_timestamp_on()

Turns timestamp reporting on. NOTE: Off by default.

#### plog_turn_timestamp_off()

Turns timestamp reporting off.

#### plog_turn_level_on()

Turns log level reporting on. NOTE: On by default.

#### plog_turn_level_off()

Turns log level reporting off

#### plog_turn_file_on()

Turns filename/line number reporting on. NOTE: Off by default.

#### plog_turn_file_off()

Turns filename/line number reporting off.

#### plog_turn_func_on()

Turns function name reporting on. NOTE: Off by default.

#### plog_turn_func_off()

Turns function name reporting off.

## License
Copyright (c) 2020 James McLean.
Licensed under the MIT license.
