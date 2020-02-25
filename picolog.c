#include "picolog.h"

#include <stdarg.h>  /* va_list, va_start, va_end */
#include <stdio.h>   /* vsnprintf */
#include <string.h>
#include <time.h>

const size_t g_timestamp_len = 32;
const size_t g_level_len     = 16;
const size_t g_file_len      = 64;
const size_t g_func_len      = 32;
const size_t g_msg_len       = PLOG_MAX_MSG_LENGTH;
const size_t g_entry_len     = g_timestamp_len +
                               g_level_len     +
                               g_file_len      +
                               g_func_len      +
                               g_msg_len;


static bool         gb_initialized   = false;
static bool         gb_enabled       = false;
static bool         gb_timestamp     = false;
static plog_level_t g_log_level      = PLOG_LEVEL_DEBUG;
static bool         gb_file          = false;
static bool         gb_func          = false;

static size_t g_appender_count = 0;

const char* const error_str_p[] =
{
    "OK",
    "Max appenders reached",
    "Invalid argument",
    "Invalid ID",
    "Appender failed",
    "Unknown",
     0
};

const char* const level_str[] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL",
    0
};

typedef struct
{
    plog_appender_t p_appender;
    void*           p_user_data;
    bool            b_enabled;
} appender_info_t;

static appender_info_t gp_appenders[PLOG_MAX_APPENDERS];

static void
try_init ()
{
    if (gb_initialized)
    {
        return;
    }

    g_log_level = PLOG_LEVEL_DEBUG;

    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        gp_appenders[i].p_appender  = NULL;
        gp_appenders[i].p_user_data = NULL;
        gp_appenders[i].b_enabled   = false;
    }

    gb_initialized = true;
}

const char*
plog_error_str(plog_error_t error_code)
{
    if (error_code < 0 || error_code >= PLOG_ERROR_COUNT)
    {
        return error_str_p[error_code];
    }
    else
    {
        return error_str_p[PLOG_ERROR_UNKNOWN];
    }
}

plog_error_t
plog_set_level (plog_level_t level)
{
    if (level < 0 || level >= PLOG_LEVEL_COUNT)
    {
        return PLOG_ERROR_INVALD_ARG;
    }

    g_log_level = level;

    return PLOG_ERROR_OK;
}

void
plog_timestamp_on ()
{
    gb_timestamp = true;
}

void
plog_timestamp_off ()
{
    gb_timestamp = true;
}

void
plog_file_on ()
{
    gb_file = true;
}

void
plog_file_off ()
{
    gb_file = false;
}

void
plog_func_on ()
{
    gb_func = true;
}

void
plog_func_off ()
{
    gb_func = false;
}

plog_error_t
plog_appender_register (plog_appender_t appender,
                        void* user_data,
                        plog_appender_id_t* id)
{
    try_init();

    if (PLOG_MAX_APPENDERS <= g_appender_count)
    {
        return PLOG_ERROR_MAX_APPENDERS;
    }

    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        if (NULL == gp_appenders[i].p_appender)
        {
            gp_appenders[i].p_appender = appender;
            gp_appenders[i].b_enabled = true;

            if (NULL != id)
            {
                *id = i;
            }

            g_appender_count++;

            return PLOG_ERROR_OK;
        }
    }

    return PLOG_ERROR_UNKNOWN;
}

plog_error_t
plog_appender_unregister (plog_appender_id_t id)
{
    try_init();

    if (PLOG_MAX_APPENDERS <= id)
    {
        return PLOG_ERROR_INVALD_ID;
    }

    if (NULL == gp_appenders[id].p_appender)
    {
        return PLOG_ERROR_INVALD_ID;
    }

    gp_appenders[id].p_appender = NULL;
    gp_appenders[id].p_user_data = NULL;
    gp_appenders[id].b_enabled = false;

    g_appender_count--;

    return PLOG_ERROR_OK;
}

plog_error_t
plog_appender_enable (plog_appender_id_t id)
{
    try_init();

    if (PLOG_MAX_APPENDERS <= id)
    {
        return PLOG_ERROR_INVALD_ID;
    }

    if (NULL == gp_appenders[id].p_appender)
    {
        return PLOG_ERROR_INVALD_ID;
    }

    gp_appenders[id].b_enabled = true;

    return PLOG_ERROR_OK;
}

plog_error_t
plog_appender_disable (plog_appender_id_t id)
{
    try_init();

    if (PLOG_MAX_APPENDERS <= id)
    {
        return PLOG_ERROR_INVALD_ID;
    }

    if (NULL == gp_appenders[id].p_appender)
    {
        return PLOG_ERROR_INVALD_ID;
    }

    gp_appenders[id].b_enabled = false;

    return PLOG_ERROR_OK;
}

void
plog_enable()
{
    gb_enabled = true;
}

void
plog_disable()
{
    gb_enabled = false;
}

static char*
time_str(char* p_str, size_t len)
{
    time_t now = time(0);
    strftime(p_str, len, "%d/%m/%g %H:%M:%S", localtime(&now));
    return p_str;
}

plog_error_t
plog_write (plog_level_t level, const char* file, unsigned line, const char* func, const char* p_fmt, ...)
{
    if (0 == g_appender_count || !gb_enabled)
    {
        return PLOG_ERROR_OK;
    }

    if (PLOG_LEVEL_COUNT <= level)
    {
        return PLOG_ERROR_INVALD_ARG;
    }

    if (g_log_level > level)
    {
        return PLOG_ERROR_OK;
    }

    char p_entry_str[g_entry_len + 1];
    p_entry_str[0] = '\0';

    if (gb_timestamp)
    {
        char p_time_str[g_timestamp_len];
        char p_tmp_str[g_timestamp_len];
        snprintf(p_time_str, sizeof(p_time_str), "[%s] ", time_str(p_tmp_str, sizeof(p_tmp_str)));
        strncat(p_entry_str, p_time_str, sizeof(p_entry_str));
    }

    char p_level_str[g_level_len];
    snprintf(p_level_str, sizeof(p_level_str), "[%s] ", level_str[level]);
    strncat(p_entry_str, p_level_str, sizeof(p_entry_str));

    if (gb_file)
    {
        char p_file_str[g_file_len];
        snprintf(p_file_str, sizeof(p_file_str), "[%s:%u] ", file, line);
        strncat(p_entry_str, p_file_str, sizeof(p_entry_str));
    }

    if (gb_func)
    {
        char p_func_str[g_func_len];
        snprintf(p_func_str, sizeof(p_func_str), "[%s] ", func);
        strncat(p_entry_str, p_func_str, sizeof(p_entry_str));
    }

    char p_msg_str[g_msg_len];

    va_list args;
    va_start(args, p_fmt);
    vsnprintf(p_msg_str, sizeof(p_msg_str), p_fmt, args);
    va_end(args);

    strncat(p_entry_str, p_msg_str, sizeof(p_entry_str));

    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        if (NULL != gp_appenders[i].p_appender && gp_appenders[i].b_enabled)
        {
            if (!gp_appenders[i].p_appender(p_entry_str, gp_appenders[i].p_user_data))
            {
                return PLOG_ERROR_APPENDER_FAILED;
            }
        }
    }

    return PLOG_ERROR_OK;
}
