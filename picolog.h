/** @file picolog.h
 * picolog is a minimal, yet flexible logging framework written in C99. Due to
 * its small footprint, it is suitable for embedded as well as general
 * software development.
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

#include <assert.h>  /* assert */
#include <stdarg.h>  /* ... */
#include <stdbool.h> /* bool, true, false */
#include <stddef.h>  /* NULL, size_t */

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
 * Writes a TRACE level message to the log.
 */
#define PLOG_TRACE(...) \
        plog_write(PLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a DEBUG level message to the log.
 */
#define PLOG_DEBUG(...) \
        plog_write(PLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes an INFO level message to the log.
 */
#define PLOG_INFO(...) \
        plog_write(PLOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a WARN level message to the log.
 */
#define PLOG_WARN(...) \
        plog_write(PLOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a ERROR level message to the log.
 */
#define PLOG_ERROR(...) \
        plog_write(PLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a FATAL level message to the log.
 */
#define PLOG_FATAL(...) \
        plog_write(PLOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Appender function definition. An appender writes a log entry to an output
 * stream. This could be the console, a file, a network connection, etc...
 */
typedef void (*plog_appender_t)(const char* p_entry, void* p_user_data);

/**
 * Identifies a registered appender.
 */
typedef size_t plog_id_t;

/**
 * Registers (adds appender to logger) and enables the specified appender.
 *
 * @param appender    The appender function to register.
 * @param p_user_data A pointer supplied to the appender function when writing
 *                    a log entry. This pointer is not modified by the logger.
 *                    If not required, pass in NULL for this parameter.
 * @param id          An identifier for the appender. If not required, pass in
 *                    NULL for this parameter.
 */
void plog_appender_register(plog_appender_t appender,
                            void* p_user_data,
                            plog_id_t* id);

/**
 * Unregisters appender (removes the appender from the logger)
 *
 * @param id The appender to unreqister.
 */
void plog_appender_unregister(plog_id_t id);

/**
 * Enables the specified appender. NOTE: Appenders are enabled by default after
 * registration.
 *
 * @param id The appender to enable.
 */
void plog_appender_enable(plog_id_t id);

/**
 * Disables the specified appender.
 *
 * @param id The appender to disable.
 */
void plog_appender_disable(plog_id_t id);

/**
 * Enables logging. NOTE: Logging is enabled by default.
 */
void plog_enable();

/**
 * Disables logging.
 */
void plog_disable();

/**
 * Sets the logging level. Only those messages of equal or higher priority
 * (severity) than this value will be logged.
 *
 * @param level The new global logging threshold.
 */
void plog_set_level(plog_level_t level);

/**
 * Turns timestamp reporting on. NOTE: Off by default.
 */
void plog_report_timestamp_on();

/**
 * Turns timestamp reporting off.
 */
void plog_report_timestamp_off();

/**
 * Turns log level reporting on. NOTE: On by default.
 */
void plog_report_level_on();

/**
 * Turns log level reporting off.
 */
void plog_report_level_off();

/**
 * Turns filename/line number reporting on. NOTE: Off by default.
 */
void plog_report_file_on();

/**
 * Turns filename/line number reporting off.
 */
void plog_report_file_off();

/**
 * Turns function name reporting on. NOTE: Off by default.
 */
void plog_report_func_on();

/**
 * Turns function name reporting off.
 */
void plog_report_func_off();

/**
 * Formats and sends a log entry to the registered appenders. This function is
 * conditionally thread-safe (provided that the appenders ARE thread-safe) in
 * the sense that it does not write to shared memory. It is, however,
 * susceptible to races conditions in that the order of log entries is
 * ultimately determined by the timing differences of the individual threads.
 *
 * NOTE: It is inadvisable to call this function directly. Use the macros
 * instead.
 */
void plog_write(plog_level_t level,
                const char* file,
                unsigned line,
                const char* func,
                const char* p_fmt, ...);


#endif /* PICOLOG_H */

/* EoF */
