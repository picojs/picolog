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
#include <stdio.h>  // vsnprintf, FILE, fprintf, fflush
#include <string.h> // strncat
#include <time.h>   // time, strftime

/*
 * Log entry component maximum sizes. These have been chosen to be overly
 * generous powers of 2 for the sake of safety and simplicity.
 */

#define PLOG_TIMESTAMP_LEN 64
#define PLOG_LEVEL_LEN     32
#define PLOG_FILE_LEN      512
#define PLOG_FUNC_LEN      32
#define PLOG_MSG_LEN       PLOG_MAX_MSG_LENGTH
#define PLOG_BREAK_LEN     1

#define PLOG_ENTRY_LEN     (PLOG_TIMESTAMP_LEN  + \
                            PLOG_LEVEL_LEN      + \
                            PLOG_FILE_LEN       + \
                            PLOG_FUNC_LEN       + \
                            PLOG_MSG_LEN        + \
                            PLOG_BREAK_LEN)

#define PLOG_TIME_FMT_LEN 32
#define PLOG_TIME_FMT     "%d/%m/%g %H:%M:%S"

#define PLOG_TERM_CODE    0x1B
#define PLOG_TERM_RESET   "[0m"
#define PLOG_TERM_GRAY    "[90m"

static plog_lock_fn gp_lock           =  NULL; // Calls the lock function
                                               // (if not NULL)
static void*        gp_lock_user_data = NULL;  // Passed to the lock function on
                                               // plog_write
static bool         gb_initialized    = false; // True if logger is initialized
static bool         gb_enabled        = true;  // True if logger is enabled

static size_t g_appender_count = 0;           // Number of appenders

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
 * Logger level strings indexed by level ID (plog_level_t).
 */
static const char* const level_str_formatted[] =
{
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL",
    0
};

// Appropriated from https://github.com/rxi/log.c (MIT licensed)
static const char* level_color[] =
{
    "[94m", "[36m", "[32m", "[33m", "[31m", "[35m", NULL
};

/*
 * Appender pointer and metadata.
 */
typedef struct
{
    plog_appender_fn p_appender;
    void*            p_user_data;
    bool             b_enabled;
    char             pad1[3];
    plog_level_t     level;
    char             p_time_fmt[PLOG_TIME_FMT_LEN];
    bool             b_colors;
    bool             b_timestamp;
    bool             b_level;
    bool             b_file;
    bool             b_func;
    char             pad2[3];
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

    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        gp_appenders[i].p_appender = NULL;
    }

    gb_initialized = true;
}

static bool appender_exists(plog_id_t id)
{
    return (id < PLOG_MAX_APPENDERS && NULL != gp_appenders[id].p_appender);
}

static bool appender_enabled(plog_id_t id)
{
    return appender_exists(id) && gp_appenders[id].b_enabled;
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
plog_enable (void)
{
    gb_enabled = true;
}

void
plog_disable (void)
{
    gb_enabled = false;
}

void plog_set_lock(plog_lock_fn p_lock, void* p_user_data)
{
    PLOG_ASSERT(NULL != p_lock);

    try_init();

    gp_lock = p_lock;
    gp_lock_user_data = p_user_data;
}

plog_id_t
plog_add_appender (plog_appender_fn p_appender,
                        plog_level_t level,
                        void* p_user_data)
{
    // Initialize logger if neccesary
    try_init();

    // Check if there is space for a new appender.
    PLOG_ASSERT(g_appender_count < PLOG_MAX_APPENDERS);

    // Ensure level is valid
    PLOG_ASSERT(level >= 0 && level < PLOG_LEVEL_COUNT);

    // Iterate through appender array and find an empty slot.
    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        if (NULL == gp_appenders[i].p_appender)
        {
            // Store and enable appender
            gp_appenders[i].p_appender  = p_appender;
            gp_appenders[i].level       = level;
            gp_appenders[i].p_user_data = p_user_data;
            gp_appenders[i].level       = PLOG_LEVEL_INFO;
            gp_appenders[i].b_enabled   = true;
            gp_appenders[i].b_colors    = false;
            gp_appenders[i].b_level     = true;
            gp_appenders[i].b_timestamp = false;
            gp_appenders[i].b_file      = false;
            gp_appenders[i].b_func      = false;

            strncpy(gp_appenders[i].p_time_fmt, PLOG_TIME_FMT, PLOG_TIME_FMT_LEN);

            g_appender_count++;

            return (plog_id_t)i;
        }
    }

    // This should never happen
    PLOG_ASSERT(false);
    return 0;
}

static void
stream_appender (const char* p_entry, void* p_user_data)
{
    FILE* stream = (FILE*)p_user_data;
    fprintf(stream, "%s", p_entry);
    fflush(stream);
}

plog_id_t
plog_add_stream (FILE* stream, plog_level_t level)
{
    // Stream must not be NULL
    PLOG_ASSERT(NULL != stream);

    return plog_add_appender(stream_appender, level, stream);
}

void
plog_remove_appender (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Reset appender with given ID
    gp_appenders[id].p_appender = NULL;

    g_appender_count--;
}

void
plog_enable_appender (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Enable appender
    gp_appenders[id].b_enabled = true;
}

void
plog_disable_appender (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Disable appender
    gp_appenders[id].b_enabled = false;
}

void
plog_set_level (plog_id_t id, plog_level_t level)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Ensure level is valid
    PLOG_ASSERT(level >= 0 && level < PLOG_LEVEL_COUNT);

    // Set the level
    gp_appenders[id].level = level;
}

void
plog_set_time_fmt (plog_id_t id, const char* fmt)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Copy the time string
    strncpy(gp_appenders[id].p_time_fmt, fmt, PLOG_TIME_FMT_LEN);
}

void
plog_colors_on (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Disable appender
    gp_appenders[id].b_colors = true;
}

void
plog_colors_off (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Disable appender
    gp_appenders[id].b_colors = false;
}

void
plog_timestamp_on (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn timestamp on
    gp_appenders[id].b_timestamp = true;
}

void
plog_timestamp_off (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn timestamp off
    gp_appenders[id].b_timestamp = false;
}

void
plog_level_on (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn level reporting on
    gp_appenders[id].b_level = true;
}

void
plog_level_off (plog_id_t id)
{    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn level reporting off
    gp_appenders[id].b_level = false;
}

void
plog_file_on (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn file reporting on
    gp_appenders[id].b_file = true;
}

void
plog_file_off (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn file reporting on
    gp_appenders[id].b_file = false;
}

void
plog_func_on (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn file reporting on
    gp_appenders[id].b_func = true;
}

void
plog_func_off (plog_id_t id)
{
    // Initialize logger if neccesary
    try_init();

    // Ensure appender is registered
    PLOG_ASSERT(appender_exists(id));

    // Turn file reporting on
    gp_appenders[id].b_func = false;
}

/*
 * Formats the current time as as string.
 */
static char*
time_str (const char* p_time_fmt, char* p_str, size_t len)
{
    time_t now = time(0);
    size_t ret = strftime(p_str, len, p_time_fmt, localtime(&now));

    PLOG_ASSERT(ret > 0);

    return p_str;
}

static void
append_timestamp (char* p_entry_str, const char* p_time_fmt)
{
    char p_time_str[PLOG_TIMESTAMP_LEN + 1];
    char p_tmp_str[PLOG_TIMESTAMP_LEN + 1];

    snprintf(p_time_str, PLOG_TIMESTAMP_LEN, "%s ",
             time_str(p_time_fmt, p_tmp_str, PLOG_TIMESTAMP_LEN));

    strncat(p_entry_str, p_time_str, PLOG_TIMESTAMP_LEN);
}

static void
append_level (char* p_entry_str, plog_level_t level, bool b_colors)
{
    char p_level_str[PLOG_LEVEL_LEN];

    if (b_colors)
    {
        snprintf(p_level_str, sizeof(p_level_str), "%c%s%s %c%s",
        PLOG_TERM_CODE, level_color[level],
        level_str_formatted[level],
        PLOG_TERM_CODE, PLOG_TERM_RESET);
    }
    else
    {
        snprintf(p_level_str, sizeof(p_level_str), "%s ", level_str[level]);
    }

    strncat(p_entry_str, p_level_str, PLOG_LEVEL_LEN);
}

static void
append_file(char* p_entry_str, const char* file, unsigned line, bool b_colors)
{
    char p_file_str[PLOG_FILE_LEN];

    if (b_colors)
    {
        snprintf(p_file_str, sizeof(p_file_str), "%c%s%s:%u%c%s ",
                 PLOG_TERM_CODE, PLOG_TERM_GRAY,
                 file, line,
                 PLOG_TERM_CODE, PLOG_TERM_RESET);

    }
    else
    {
        snprintf(p_file_str, sizeof(p_file_str), "%s:%u ", file, line);
    }

    strncat(p_entry_str, p_file_str, PLOG_FILE_LEN);
}

static void
append_func(char* p_entry_str, const char* func, bool b_colors)
{
   char p_func_str[PLOG_FUNC_LEN];

    if (b_colors)
    {
        snprintf(p_func_str, sizeof(p_func_str), "%c%s[%s] %c%s",
                 PLOG_TERM_CODE, PLOG_TERM_GRAY,
                 func,
                 PLOG_TERM_CODE, PLOG_TERM_RESET);
    }
    else
    {
        snprintf(p_func_str, sizeof(p_func_str), "[%s] ", func);
    }

    strncat(p_entry_str, p_func_str, PLOG_FUNC_LEN);
}

void
plog_write (plog_level_t level, const char* file, unsigned line,
                                const char* func, const char* p_fmt, ...)
{
    // Calls the lock function (if the lock function is set)
    if (NULL != gp_lock)
    {
        gp_lock(true, gp_lock_user_data);
    }

    // Only write entry if there are registered appenders and the logger is
    // enabled
    if (0 == g_appender_count || !gb_enabled)
    {
        return;
    }

    // Ensure valid log level
    PLOG_ASSERT(level < PLOG_LEVEL_COUNT);

    for (plog_id_t i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        if (appender_enabled(i) &&
            gp_appenders[i].level <= level)
        {
            char p_entry_str[PLOG_ENTRY_LEN + 1]; // Ensure there is space for
                                                  // null char

            p_entry_str[0] = '\0'; // Ensure the entry is null terminated

            // Append a timestamp
            if (gp_appenders[i].b_timestamp)
            {
                append_timestamp(p_entry_str, gp_appenders[i].p_time_fmt);
            }

            // Append the logger level
            if (gp_appenders[i].b_level)
            {
                append_level(p_entry_str, level, gp_appenders[i].b_colors);
            }

            // Append the filename/line number
            if (gp_appenders[i].b_file)
            {
                append_file(p_entry_str, file, line, gp_appenders[i].b_colors);
            }

            // Append the function name
            if (gp_appenders[i].b_func)
            {
                append_func(p_entry_str, func, gp_appenders[i].b_colors);
            }

            // Append the log message
            char p_msg_str[PLOG_MSG_LEN];

            va_list args;
            va_start(args, p_fmt);
            vsnprintf(p_msg_str, sizeof(p_msg_str), p_fmt, args);
            va_end(args);

            strncat(p_entry_str, p_msg_str, PLOG_MSG_LEN);
            strcat(p_entry_str, "\n");

            gp_appenders[i].p_appender(p_entry_str, gp_appenders[i].p_user_data);
        }
    }

    // Calls the lock function (if the lock function is set)
    if (NULL != gp_lock)
    {
        gp_lock(false, gp_lock_user_data);
    }
}

/* EoF */
