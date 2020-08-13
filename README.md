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

#### plog_enable()

Enables logging. **NOTE:** Logging is enabled by default.

#### plog_disable()

Disables logging.

#### plog_set_lock(plog_lock_fn p_lock, void* p_userdata)

Registers a lock function that is called while writing to the log. **NOTE:** Off by default.

- `p_lock` - The lock function

- `p_userdata` - User data that is supplied to the lock function during execution.

#### plog_add_appender(p_appender, level, p_user_data)

Registers (adds appender to logger) and enables the specified appender. An
appender writes a log entry to an output stream. This could be a console,
a file, a network connection, etc...

- `p_appender`  - Pointer to the appender function to register. An appender
                  function has the signature,
                  `void appender_func(const char* p_entry, void* p_user_data)`

- `level`       - The logging threshold for the appender

- `p_user_data` - A pointer supplied to the appender function when writing a log
                  entry. This pointer is not modified by the logger. If not
                  required, pass in NULL for this parameter<br/>

**returns**       An identifier for the appender. This ID is valid until the
                  appender is unregistered.

#### plog_add_stream(p_stream, level)

Registers an output stream appender.

- `stream`  - The output stream to write to

- `level`   - The logging threshold for the appender

**returns** An identifier for the appender. This ID is valid until the
            appender is unregistered.

#### plog_remove_appender(id)

Unregisters appender (removes the appender from the logger).

- `id` - The appender to unreqister

#### plog_enable_appender(id)

Enables the specified appender. **NOTE:** Appenders are enabled by default after
registration.

- `id` - The appender to enable

#### plog_disable_appender(id)

Disables the specified appender.

- `id` - The appender to disable

#### plog_set_level(level)

Sets the logging level. Only those messages of equal or higher priority
(severity) than this value will be logged.

- `level` - The new global logging threshold.

#### plog_trace(fmt, args...)

Writes a TRACE level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### plog_debug(fmt, args...)

Writes a DEBUG level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### plog_info(fmt, args...)

Writes a INFO level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### plog_warn(fmt, args...)

Writes a WARN level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### plog_error(fmt, args...)

Writes a ERROR level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### plog_fatal(fmt, args...)

Writes a FATAL level message to the log. This macro behaves identically to
printf.

- `fmt`     - Message format
- `args...` - Format specifiers

#### plog_turn_colors_on(id)

Turns color output on for the specified appender. NOTE: Off by default.

- `id`     - The appender id

#### plog_turn_colors_off()

Turns color ouput off for the specified appender.

- `id`     - The appender id

#### plog_turn_timestamp_on()

Turns timestamp reporting on. **NOTE:** Off by default.

#### plog_turn_timestamp_off()

Turns timestamp reporting off.

#### plog_turn_level_on()

Turns log level reporting on. **NOTE:** On by default.

#### plog_turn_level_off()

Turns log level reporting off

#### plog_turn_file_on()

Turns filename/line number reporting on. **NOTE:** Off by default.

#### plog_turn_file_off()

Turns filename/line number reporting off.

#### plog_turn_func_on()

Turns function name reporting on. **NOTE:** Off by default.

#### plog_turn_func_off()

Turns function name reporting off.

Example:
--------

```C
#include <stdio.h>

#include "../picolog.h"

void appender(const char* p_msg, void* p_user_data)
{
    printf("%s\n", p_msg);
}

int main(int argc, char** argv)
{
    plog_appender_register(appender, PLOG_LEVEL_INFO, NULL);

    plog_turn_timestamp_on();
    plog_turn_file_on();
    plog_turn_func_on();

    // Default log level is INFO

    plog_trace ("Test message: %d", 0);
    plog_debug ("Test message: %d", 1);
    plog_info  ("Test message: %d", 2);
    plog_warn  ("Test message: %d", 3);
    plog_error ("Test message: %d", 4);
    plog_fatal ("Test message: %d", 5);

    return 0;
}
```

Output:
--------

>[29/02/20 14:35:42] [INFO ] [example1.c:20] [main] Test message: 2<br/>
>[29/02/20 14:35:42] [WARN ] [example1.c:21] [main] Test message: 3<br/>
>[29/02/20 14:35:42] [ERROR] [example1.c:22] [main] Test message: 4<br/>
>[29/02/20 14:35:42] [FATAL] [example1.c:23] [main] Test message: 5

## License
Copyright (c) 2020 James McLean<br/>
Licensed under the MIT license
