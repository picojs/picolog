/** @file picolog.h
 * picounit is a minimal, yet flexible logging framework written in C99. Due to
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

#include <stdarg.h>
#include <stdbool.h> /* bool, true, false */
#include <stddef.h>  /* NULL, size_t */

/*
 * Configuration constants.
 */
#define PLOG_MAX_APPENDERS  8
#define PLOG_MAX_MSG_LENGTH 512

typedef enum
{
    PLOG_ERROR_OK = 0,
    PLOG_ERROR_MAX_APPENDERS,
    PLOG_ERROR_INVALD_ARG,
    PLOG_ERROR_INVALD_ID,
    PLOG_ERROR_APPENDER_FAILED,
    PLOG_ERROR_UNKNOWN, /* This should never happen */
    PLOG_ERROR_COUNT,
} plog_error_t;

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

#define PLOG_TRACE(...) \
        plog_write(PLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define PLOG_DEBUG(...) \
        plog_write(PLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define PLOG_INFO(...) \
        plog_write(PLOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)

#define PLOG_WARN(...) \
        plog_write(PLOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)

#define PLOG_ERROR(...) \
        plog_write(PLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define PLOG_FATAL(...) \
        plog_write(PLOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

typedef bool   (*plog_appender_t)(const char* p_msg, void* p_user_data);
typedef size_t   plog_appender_id_t;

const char*
plog_error_str(plog_error_t error_code);

plog_error_t plog_set_level(plog_level_t level);

void plog_timestamp_on();

void plog_timestamp_off();

void plog_file_on();

void plog_file_off();

void plog_func_on();

void plog_func_off();

plog_error_t plog_appender_register(plog_appender_t appender,
                                    void* p_user_data,
                                    plog_appender_id_t* id);

plog_error_t plog_appender_unregister(plog_appender_id_t id);

plog_error_t plog_appender_enable(plog_appender_id_t id);

plog_error_t plog_appender_disable(plog_appender_id_t id);

void plog_enable();

void plog_disable();

/*
 * NOTE: It is inadvisable to call this function directly. Use the macros
 * instead.
 */

plog_error_t plog_write(plog_level_t level,
                        const char* file,
                        unsigned line,
                        const char* func,
                        const char* p_fmt, ...);


#endif /* PICOLOG_H */

/* EoF */
