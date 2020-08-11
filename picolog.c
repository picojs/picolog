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

/*
 * Implementation
 */

#include "picolog.h"

#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h>  // vsnprintf
#include <string.h> // strncat
#include <time.h>   // time, strftime

/*
 * Log entry component maximum sizes. These have been chosen to be overly
 * generous powers of 2 for the sake of safety and simplicity.
 */

#define PLOG_TIMESTAMP_LEN 32
#define PLOG_LEVEL_LEN     16
#define PLOG_FILE_LEN      64
#define PLOG_FUNC_LEN      32
#define PLOG_MSG_LEN       PLOG_MAX_MSG_LENGTH
#define PLOG_ENTRY_LEN    (PLOG_TIMESTAMP_LEN + \
                           PLOG_LEVEL_LEN     + \
                           PLOG_FILE_LEN      + \
                           PLOG_FUNC_LEN      + \
                           PLOG_MSG_LEN)

static bool         gb_initialized   = false; // True if logger is initialized
static bool         gb_enabled       = true;  // True if logger is enabled
static bool         gb_timestamp     = false; // True if timestamps are on
static bool         gb_level         = true;  // True is level reporting is on
static plog_level_t g_log_level      = PLOG_LEVEL_DEBUG; // Logger level
static bool         gb_file          = false; // True if filenames/lines are on
static bool         gb_func          = false; // True if function names are on

static size_t g_appender_count = 0; // Number of registered appenders

/*
 * Logger level strings indexed by level ID (plog_level_t).
 */
static const char* const level_str[] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
    0
};

/*
 * Appender pointer and metadata.
 */
typedef struct
{
    plog_appender_t p_appender;
    void*           p_userdata;
    bool            b_enabled;
    char            padding[7];
} appender_info_t;

/*
 * Array of appenders.
 */
static appender_info_t gp_appenders[PLOG_MAX_APPENDERS];

/*
 * Initializes the logger provided it has not been initialized.
 */
static void
try_init ()
{
    if (gb_initialized)
    {
        return;
    }

    g_log_level = PLOG_LEVEL_INFO; // Default logger level is INFO

    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        gp_appenders[i].p_appender  = NULL;
        gp_appenders[i].p_userdata = NULL;
        gp_appenders[i].b_enabled   = false;
    }

    gb_initialized = true;
}

bool plog_str_level(const char* str, plog_level_t* level)
{
    if (!level)
        return false;

    for (size_t i = 0; level_str[i]; i++)
    {
        if (0 == strcmp(str, level_str[i]))
        {
            *level = (plog_level_t)i;
            return true;
        }
    }

    return false;
}

void
plog_enable ()
{
    gb_enabled = true;
}

void
plog_disable ()
{
    gb_enabled = false;
}

plog_id_t
plog_appender_register (plog_appender_t p_appender, void* p_userdata)
{
    // Initialize logger if neccesary
    try_init();

    // Check if there is space for a new appender.
    PLOG_ASSERT(g_appender_count < PLOG_MAX_APPENDERS);

    // Iterate through appender array and find an empty slot.
    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        if (NULL == gp_appenders[i].p_appender)
        {
            // Store and enable appender
            gp_appenders[i].p_appender  = p_appender;
            gp_appenders[i].p_userdata = p_userdata;
            gp_appenders[i].b_enabled   = true;

            g_appender_count++;

            return (plog_id_t)i;
        }
    }

    // This should never happen
    PLOG_ASSERT(false);
}

void
plog_appender_unregister (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure ID is valid
    PLOG_ASSERT(id < PLOG_MAX_APPENDERS);

    // Ensure appender is registered
    PLOG_ASSERT(NULL != gp_appenders[id].p_appender);

    // Reset appender with given ID
    gp_appenders[id].p_appender  = NULL;
    gp_appenders[id].p_userdata = NULL;
    gp_appenders[id].b_enabled   = false;

    g_appender_count--;
}

void
plog_appender_enable (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure ID is valid
    PLOG_ASSERT(id < PLOG_MAX_APPENDERS);

    // Ensure appender is registered
    PLOG_ASSERT(NULL != gp_appenders[id].p_appender);

    // Enable appender
    gp_appenders[id].b_enabled = true;
}

void
plog_appender_disable (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure ID is valid
    PLOG_ASSERT(id < PLOG_MAX_APPENDERS);

    // Ensure appender is registered
    PLOG_ASSERT(NULL != gp_appenders[id].p_appender);

    // Disable appender
    gp_appenders[id].b_enabled = false;
}

bool plog_appender_enabled(plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure ID is valid
    PLOG_ASSERT(id < PLOG_MAX_APPENDERS);

    // Ensure appender is registered
    PLOG_ASSERT(NULL != gp_appenders[id].p_appender);

    return gp_appenders[id].b_enabled;
}

void plog_appender_set_userdata(plog_id_t id, void* p_userdata)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure ID is valid
    PLOG_ASSERT(id < PLOG_MAX_APPENDERS);

    // Ensure appender is registered
    PLOG_ASSERT(NULL != gp_appenders[id].p_appender);

    gp_appenders[id].p_userdata = p_userdata;
}


void
plog_set_level (plog_level_t level)
{
    // Ensure level is valid
    PLOG_ASSERT(level >= 0 && level < PLOG_LEVEL_COUNT);

    g_log_level = level;
}

void
plog_turn_timestamp_on ()
{
    gb_timestamp = true;
}

void
plog_turn_timestamp_off ()
{
    gb_timestamp = true;
}

void
plog_turn_level_on ()
{
    gb_level = true;
}

void
plog_turn_level_off ()
{
    gb_level = false;
}

void
plog_turn_file_on ()
{
    gb_file = true;
}

void
plog_turn_file_off ()
{
    gb_file = false;
}

void
plog_turn_func_on ()
{
    gb_func = true;
}

void
plog_turn_func_off ()
{
    gb_func = false;
}

/*
 * Formats the current time as as string.
 */
static char*
time_str (char* p_str, size_t len)
{
    time_t now = time(0);
    strftime(p_str, len, "%d/%m/%g %H:%M:%S", localtime(&now));
    return p_str;
}

void
plog_write (plog_level_t level, const char* file, unsigned line,
                                const char* func, const char* p_fmt, ...)
{
    // Only write entry if there are registered appenders and the logger is
    // enabled
    if (0 == g_appender_count || !gb_enabled)
    {
        return;
    }

    // Ensure valid log level
    PLOG_ASSERT(level < PLOG_LEVEL_COUNT);

    // Only write the log entry if the current logger level is less than or
    // equal to the specified level
    if (g_log_level > level)
    {
        return;
    }

    char p_entry_str[PLOG_ENTRY_LEN + 1]; // Ensure there is space for null char
    p_entry_str[0] = '\0'; // Ensure the entry is null terminated

    // Append the timestamp
    if (gb_timestamp)
    {
        char p_time_str[PLOG_TIMESTAMP_LEN];
        char p_tmp_str[PLOG_TIMESTAMP_LEN];
        snprintf(p_time_str, sizeof(p_time_str), "[%s] ",
                 time_str(p_tmp_str, sizeof(p_tmp_str)));

        strncat(p_entry_str, p_time_str, sizeof(p_entry_str) - 1);
    }

    // Append the logger level
    if (gb_level)
    {
        char p_level_str[PLOG_LEVEL_LEN];
        snprintf(p_level_str, sizeof(p_level_str), "[%s] ", level_str[level]);
        strncat(p_entry_str, p_level_str, sizeof(p_entry_str) - 1);
    }

    // Append the filename/line number
    if (gb_file)
    {
        char p_file_str[PLOG_FILE_LEN];
        snprintf(p_file_str, sizeof(p_file_str), "[%s:%u] ", file, line);
        strncat(p_entry_str, p_file_str, sizeof(p_entry_str) - 1);
    }

    // Append the function name
    if (gb_func)
    {
        char p_func_str[PLOG_FUNC_LEN];
        snprintf(p_func_str, sizeof(p_func_str), "[%s] ", func);
        strncat(p_entry_str, p_func_str, sizeof(p_entry_str) - 1);
    }

    // Append the log message
    char p_msg_str[PLOG_MSG_LEN];

    va_list args;
    va_start(args, p_fmt);
    vsnprintf(p_msg_str, sizeof(p_msg_str), p_fmt, args);
    va_end(args);

    strncat(p_entry_str, p_msg_str, sizeof(p_entry_str) - 1);

    // Send the finished entry to all enabled appenders
    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        if (gp_appenders[i].b_enabled)
        {
            gp_appenders[i].p_appender(p_entry_str, gp_appenders[i].p_userdata);
        }
    }
}

/* EoF */
