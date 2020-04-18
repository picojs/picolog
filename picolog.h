/** @file picolog.h
 * picolog is a minimal, yet flexible logging framework written in C99.
 */

/*=============================================================================
 * MIT License
 *
 * Copyright (c) 2020 James McLean
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
=============================================================================*/

#ifndef PICOLOG_H
#define PICOLOG_H

#include <assert.h>  // assert
#include <stdarg.h>  // ...
#include <stdbool.h> // bool, true, false
#include <stddef.h>  // NULL, size_t

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Configuration constants/macros.
 */
#define PLOG_MAX_APPENDERS  8
#define PLOG_MAX_MSG_LENGTH 512
#define PLOG_ASSERT(expr)   assert(expr)

/**
 * These codes allow different layers of granularity when logging. See the
 * documentation of the `plog_set_level` function for more information.
 */
typedef enum
{
    PLOG_LEVEL_TRACE = 0,
    PLOG_LEVEL_DEBUG,
    PLOG_LEVEL_INFO,
    PLOG_LEVEL_WARN,
    PLOG_LEVEL_ERROR,
    PLOG_LEVEL_FATAL,
    PLOG_LEVEL_COUNT
} plog_level_t;

/**
 * Appender function definition. An appender writes a log entry to an output
 * stream. This could be the console, a file, a network connection, etc...
 */
typedef void (*plog_appender_t)(const char* p_entry, void* p_userdata);

/**
 * Identifies a registered appender.
 */
typedef size_t plog_id_t;

/**
  * Converts a string to the corresponding log level
  */
bool plog_str_level(const char* str, plog_level_t* level);

/**
 * Enables logging. NOTE: Logging is enabled by default.
 */
void plog_enable();

/**
 * Disables logging.
 */
void plog_disable();

/**
 * Registers (adds appender to logger) and enables the specified appender. An
 * appender writes a log entry to an output stream. This could be a console,
 * a file, a network connection, etc...
 *
 * @param p_appender  Pointer to the appender function to register. An appender
 *                    function has the signature,
 *                    `void appender_func(const char* p_entry, void* p_userdata)`

 * @param p_userdata A pointer supplied to the appender function when writing
 *                    a log entry. This pointer is not modified by the logger.
 *                    If not required, pass in NULL for this parameter.
 *
 * @return            An identifier for the appender. This ID is valid until the
 *                    appender is unregistered.
 */
plog_id_t plog_appender_register(plog_appender_t p_appender, void* p_userdata);

/**
 * Unregisters appender (removes the appender from the logger).
 *
 * @param id The appender to unreqister
 */
void plog_appender_unregister(plog_id_t id);

/**
 * Enables the specified appender. NOTE: Appenders are enabled by default
 * after registration.
 *
 * @param id The appender to enable.
 */
void plog_appender_enable(plog_id_t id);

/**
 * Disables the specified appender.
 *
 * @param id The appender to disable
 */
void plog_appender_disable(plog_id_t id);

/**
 * Returns true if the specified appender is enabled, and false otherwise.
 * NOTE: Appenders are enabled by default after registration.
 *
 * @param id The appender to enable.
 */
bool plog_appender_enabled(plog_id_t id);

/**
 * Sets an appender's user data.
 * @param id The appender
 * @param p_userdata The user data to set.
 */
void plog_appender_set_userdata(plog_id_t id, void* p_userdata);

/**
 * Sets the logging level. Only those messages of equal or higher priority
 * (severity) than this value will be logged.
 *
 * @param level The new global logging threshold.
 */
void plog_set_level(plog_level_t level);

/**
 * Writes a TRACE level message to the log. Usage is similar to printf (i.e.
 * PLOG_TRACE(format, args...))
 */
#define PLOG_TRACE(...) \
        plog_write(PLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a DEBUG level message to the log. Usage is similar to printf (i.e.
 * PLOG_DEBUG(format, args...))
 */
#define PLOG_DEBUG(...) \
        plog_write(PLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes an INFO level message to the log. Usage is similar to printf (i.e.
 * PLOG_INFO(format, args...))
 */
#define PLOG_INFO(...) \
        plog_write(PLOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a WARN level message to the log. Usage is similar to printf (i.e.
 * PLOG_WARN(format, args...))
 */
#define PLOG_WARN(...) \
        plog_write(PLOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a ERROR level message to the log. Usage is similar to printf (i.e.
 * PLOG_ERROR(format, args...))
 */
#define PLOG_ERROR(...) \
        plog_write(PLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a FATAL level message to the log.. Usage is similar to printf (i.e.
 * PLOG_FATAL(format, args...))
 */
#define PLOG_FATAL(...) \
        plog_write(PLOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Turns timestamp reporting on. NOTE: Off by default.
 */
void plog_turn_timestamp_on();

/**
 * Turns timestamp reporting off.
 */
void plog_turn_timestamp_off();

/**
 * Turns log level reporting on. NOTE: On by default.
 */
void plog_turn_level_on();

/**
 * Turns log level reporting off.
 */
void plog_turn_level_off();

/**
 * Turns filename/line number reporting on. NOTE: Off by default.
 */
void plog_turn_filename_on();

/**
 * Turns filename/line number reporting off.
 */
void plog_turn_filename_off();

/**
 * Turns function name reporting on. NOTE: Off by default.
 */
void plog_turn_func_on();

/**
 * Turns function name reporting off.
 */
void plog_turn_func_off();

/**
 * Formats and sends a log entry to the enabled appenders. This function is
 * conditionally thread-safe in the sense that it does not write to shared
 * memory. This condition only holds if the appenders are themselves
 * (unconditionally) thread-safe. It is, however, susceptible to race conditions
 * in that the order of log entries is ultimately determined by the timing
 * differences of the individual threads.
 *
 * **WARN:** It is inadvisable to call this function directly. Use the macros
 * instead.
 */
void plog_write(plog_level_t level,
                const char* file,
                unsigned line,
                const char* func,
                const char* p_fmt, ...);


#ifdef __cplusplus
}
#endif

#endif /* PICOLOG_H */

/* EoF */
