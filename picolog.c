#include "picolog.h"

#include <stdarg.h>
#include <stdbool.h> /* bool, true, false */
#include <stdio.h>

static bool   gb_initialized   = false;
static bool   gb_enabled       = false;
static int    g_log_level      = PLOG_LEVEL_DEBUG;
static size_t g_appender_count = 0;

const char* const error_strs[] =
{
    "OK",
    "Out of range",
    "Out of space",
    "Invalid argument",
    "Invalid ID",
    "Unknown",
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
    gb_enabled = true;
}

const char*
plog_error_str(plog_error_t error_code)
{
    if (error_code < 0 || error_code >= PLOG_ERROR_COUNT)
    {
        return error_strs[(size_t)error_code];
    }
    else
    {
        return error_strs[(size_t)PLOG_ERROR_UNKNOWN];
    }
}

void
plog_enable()
{
    try_init();
    gb_enabled = true;
}

void
plog_disable()
{
    try_init();
    gb_enabled = true;
}

plog_error_t
plog_set_level (plog_level_t level)
{
    try_init();

    if (level < 0 || level >= PLOG_LEVEL_COUNT)
    {
        return PLOG_ERROR_OUT_OF_RANGE;
    }

    g_log_level = level;

    return PLOG_ERROR_OK;
}

plog_error_t
plog_appender_register (plog_appender_t appender,
                        void* user_data,
                        plog_appender_id_t* id)
{
    try_init();

    if (PLOG_MAX_APPENDERS <= g_appender_count)
    {
        return PLOG_ERROR_OUT_OF_SPACE;
    }

    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        if (NULL == gp_appenders[i].p_appender)
        {
            gp_appenders[i].p_appender = appender;

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

    if (PLOG_ERROR_INVALD_ID <= id)
    {
        return PLOG_ERROR_OUT_OF_RANGE;
    }

    gp_appenders[id].p_appender = NULL;
    gp_appenders[id].p_user_data = NULL;
    gp_appenders[id].b_enabled = false;

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

plog_error_t
plog_write (plog_level_t level, const char* p_fmt, ...)
{
    try_init();

    if (g_log_level <= level)
    {
        char p_msg[PLOG_MAX_MSG_LENGTH];

        va_list args;
        va_start(args, p_fmt);
        vsnprintf(p_msg, sizeof(p_msg), p_fmt, args);
        va_end(args);

        for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
        {
            if (NULL != gp_appenders[i].p_appender)
            {
                gp_appenders[i].p_appender(p_msg, gp_appenders[i].p_user_data);
            }
        }
    }

    return PLOG_ERROR_OK;
}
