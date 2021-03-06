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

#ifndef PLOG_ASSERT
#include <assert.h>  // assert
#endif

#include <stdarg.h>  // ...
#include <stdbool.h> // bool, true, false
#include <stddef.h>  // NULL, size_t
#include <stdio.h>   // FILE

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Configuration constants/macros.
 */
#ifndef PLOG_MAX_APPENDERS
#define PLOG_MAX_APPENDERS 16
#endif

#ifndef PLOG_MAX_MSG_LENGTH
#define PLOG_MAX_MSG_LENGTH 1024
#endif

#ifndef PLOG_ASSERT
#define PLOG_ASSERT(expr)   assert(expr)
#endif

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
typedef void (*plog_appender_fn)(const char* p_entry, void* p_udata);

/**
 *  Lock function definition. This is called during plog_write. Adapted
    from https://github.com/rxi/log.c/blob/master/src/log.h
 */
typedef void (*plog_lock_fn)(bool lock, void *p_udata);

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
void plog_enable(void);

/**
 * Disables logging.
 */
void plog_disable(void);

/**
 * Registers (adds appender to logger) and enables the specified appender. An
 * appender writes a log entry to an output stream. This could be a console,
 * a file, a network connection, etc...
 *
 * @param p_appender  Pointer to the appender function to register. An appender
 *                    function has the signature,
 *                    `void appender_func(const char* p_entry, void* p_udata)`
 *
 * @param level       The appender's log level
 *
 * @param p_udata A pointer supplied to the appender function when writing
 *                    a log entry. This pointer is not modified by the logger.
 *                    If not required, pass in NULL for this parameter.
 *
 * @return            An identifier for the appender. This ID is valid until the
 *                    appender is unregistered.
 */
plog_id_t plog_add_appender(plog_appender_fn p_appender,
                            plog_level_t level,
                            void* p_udata);

/**
 * Registers an output stream appender.
 *
 * @param p_stream The output stream to write to
 * @param level  The appender's log level
 *
 * @return       An identifier for the appender. This ID is valid until the
 *               appender is unregistered.
 */
plog_id_t plog_add_stream(FILE* p_stream, plog_level_t level);

/**
 * Unregisters appender (removes the appender from the logger).
 *
 * @param id The appender to unregister
 */
void plog_remove_appender(plog_id_t id);

/**
 * Enables the specified appender. NOTE: Appenders are enabled by default
 * after registration.
 *
 * @param id The appender to enable.
 */
void plog_enable_appender(plog_id_t id);

/**
 * Disables the specified appender.
 *
 * @param id The appender to disable
 */
void plog_disable_appender(plog_id_t id);

/**
 * Sets the locking function.
 */
void plog_set_lock(plog_id_t id, plog_lock_fn p_lock, void* p_udata);

/**
 * Sets the logging level. Only those messages of equal or higher priority
 * (severity) than this value will be logged.
 *
 * @param level The new appender logging threshold.
 */
void plog_set_level(plog_id_t id, plog_level_t level);

/**
 * Sets the appender timestamp format according to:
 * https://man7.org/linux/man-pages/man3/strftime.3.html
 *
 * @param id The appender id
 * @param fmt The time format
 */
void plog_set_time_fmt(plog_id_t id, const char* fmt);

/**
 * Turns color ouput on for the specified appender.
 * NOTE: Off by default.
 *
 * @param id The appender id
 */
void plog_colors_on(plog_id_t id);

/**
 * Turns colors ouput off for the specified appender.
 *
 * @param id The appender id
 */
void plog_colors_off(plog_id_t id);

/**
 * Turns timestamp reporting off for the specified appender.
 * NOTE: Off by default
 *
 * @param id The appender id
 */
void plog_timestamp_on(plog_id_t id);

/**
 * Turns timestamp reporting off for the specified appender.
 *
 * @param id The appender id
 */
void plog_timestamp_off(plog_id_t id);

/**
 * Turns log level reporting on for the specified appender.
 * NOTE: On by default.
 *
 * @param id The appender id
 *
 */
void plog_level_on(plog_id_t id);

/**
 * Turns level reporting off for the specified appender.
 *
 * @param id The appender id
 */
void plog_level_off(plog_id_t id);

/**
 * Turns filename and line number reporting on for the specified appender.
 * NOTE: Off by default.
 *
 * @param id The appender id
 */
void plog_file_on(plog_id_t id);

/**
 * Turns filename/line number reporting off for the specified appender.
 *
 * @param id The appender id
 */
void plog_file_off(plog_id_t id);

/**
 * Turns function reporting on for the specified appender.
 * NOTE: Off by default.
 *
 * @param id The appender id
 */
void plog_func_on(plog_id_t id);

/**
 * Turns function reporting off for the specified appender.
 *
 * @param id The appender id
 */
void plog_func_off(plog_id_t id);

/**
 * Writes a TRACE level message to the log. Usage is similar to printf (i.e.
 * PLOG_TRACE(format, args...))
 */
#define plog_trace(...) \
        plog_write(PLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a DEBUG level message to the log. Usage is similar to printf (i.e.
 * PLOG_DEBUG(format, args...))
 */
#define plog_debug(...) \
        plog_write(PLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes an INFO level message to the log. Usage is similar to printf (i.e.
 * PLOG_INFO(format, args...))
 */
#define plog_info(...) \
        plog_write(PLOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a WARN level message to the log. Usage is similar to printf (i.e.
 * PLOG_WARN(format, args...))
 */
#define plog_warn(...) \
        plog_write(PLOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a ERROR level message to the log. Usage is similar to printf (i.e.
 * PLOG_ERROR(format, args...))
 */
#define plog_error(...) \
        plog_write(PLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * Writes a FATAL level message to the log.. Usage is similar to printf (i.e.
 * PLOG_FATAL(format, args...))
 */
#define plog_fatal(...) \
        plog_write(PLOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)


/**
 * WARNING: It is inadvisable to call this function directly. Use the macros
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
