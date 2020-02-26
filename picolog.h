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

#include <stdarg.h>  /* ... */
#include <stdbool.h> /* bool, true, false */
#include <stddef.h>  /* NULL, size_t */

/*
 * Configuration constants.
 */
#define PLOG_MAX_APPENDERS  8
#define PLOG_MAX_MSG_LENGTH 512

/**
 * Various error codes.
 */
typedef enum
{
    PLOG_ERROR_OK = 0,
    PLOG_ERROR_MAX_APPENDERS,
    PLOG_ERROR_INVALD_ARG,
    PLOG_ERROR_INVALD_ID,
    PLOG_ERROR_APPENDER_FAILED,
    PLOG_ERROR_UNKNOWN, // This should never happen
    PLOG_ERROR_COUNT,
} plog_error_t;

/**
 * These codes allow different layers of granularity when logging. See the
 * documentation for the function `plog_set_level` for more information.
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
 * Writes a TRACE level entry to the log.
 */
#define PLOG_TRACE(...) \
        plog_write(PLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a DEBUG level entry to the log.
 */
#define PLOG_DEBUG(...) \
        plog_write(PLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes an INFO level entry to the log.
 */
#define PLOG_INFO(...) \
        plog_write(PLOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a WARN level entry to the log.
 */
#define PLOG_WARN(...) \
        plog_write(PLOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a ERROR level entry to the log.
 */
#define PLOG_ERROR(...) \
        plog_write(PLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a FATAL level entry to the log.
 */
#define PLOG_FATAL(...) \
        plog_write(PLOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Appender function definition. An appender writes a log entry to an output
 * stream.
 */
typedef bool (*plog_appender_t)(const char* p_entry, void* p_user_data);

/**
 * Identifies a registered appender.
 */
typedef size_t plog_appender_id_t;

/**
 * Returns a descriptive error message corresponding to the specified error
 * code.
 */
const char*
plog_error_str(plog_error_t error_code);

/**
 * Registers (adds appender to logger) and enables the specified appender.
 *
 * @param appender    The appender function to register.
 * @param p_user_data A pointer supplied to the appender function when writing
 *                    a log entry. This pointer is not modified by the logger.
 *                    If not required pass in NULL for this parameter.
 * @param id          An identifier for the logger. If not required pass in NULL
 *                    for this parameter.
 * @return            An error code on failure
 */
plog_error_t plog_appender_register(plog_appender_t appender,
                                    void* p_user_data,
                                    plog_appender_id_t* id);

/**
 * Unregisters appender (removes the appender from the logger)
 *
 * @param id The appender to unreqister.
 * @return An error code on failure
 */
plog_error_t plog_appender_unregister(plog_appender_id_t id);

/**
 * Enables the specified appender. NOTE: Appenders are enabled by default after
 * registration.
 *
 * @param id The logger to enable.
 * @return   An error code on failure.
 */
plog_error_t plog_appender_enable(plog_appender_id_t id);

/**
 * Disables the specified appender.
 *
 * @param id The logger to disable.
 * @return   An error code on failure.
 */
plog_error_t plog_appender_disable(plog_appender_id_t id);

/**
 * Enables logging. Note: Logging is enabled by default.
 */
void plog_enable();

/**
 * Disables logging.
 */
void plog_disable();

/**
 * Sets the logging level. Only those messages of equal or higher priority
 * (severity) will be logged.
 *
 * @param level The new global logging threshold
 * @return      An error code on failure.
 */
plog_error_t plog_set_level(plog_level_t level);

/**
 * Turns timestamps on. NOTE: Off by default.
 */
void plog_timestamp_on();

/**
 * Turns timestamps off.
 */
void plog_timestamp_off();

/**
 * Turns filenames/line numbers on. Note: Off by default.
 */
void plog_file_on();

/**
 * Turns filenames/line numbers off.
 */
void plog_file_off();

/**
 * Turns function reporting on. Note: Off by default.
 */
void plog_func_on();

/**
 * Turns function reporting off.
 */
void plog_func_off();

/**
 * Formats and sends a log entry to registered appenders. This function is
 * conditionally thread-safe (provided that the appenders ARE thread-safe) in
 * the sense that it does not write to shared memory. It is, however,
 * susceptible to races conditions in that the order of log entries is
 * ultimately determined by the timing differencs of the individual threads.
 *
 * Note: It is inadvisable to call this function directly. Use the macros
 * instead.
 */
plog_error_t plog_write(plog_level_t level,
                        const char* file,
                        unsigned line,
                        const char* func,
                        const char* p_fmt, ...);


#endif /* PICOLOG_H */

/* EoF */
