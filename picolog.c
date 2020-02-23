#include "picolog.h"

typedef struct
{
    plog_appender_t p_appender;
    void*              p_user_data;
    bool               b_enabled;
} appender_info_t;

static int g_log_level  = PLOG_LEVEL_DEBUG;

static appender_info_t gp_appenders[PLOG_MAX_APPENDERS];
static size_t g_appender_count = 0;

void plog_init()
{
    g_log_level = PLOG_LEVEL_DEBUG;

    for (int i = 0; i < PLOG_MAX_APPENDERS; i++)
    {
        gp_appenders[i].p_appender  = NULL;
        gp_appenders[i].p_user_data = NULL;
        gp_appenders[i].b_enabled   = false;
    }
}

plog_error_t
plog_set_level(plog_level_t level)
{
    if (level < 0 || level >= PLOG_LEVEL_COUNT)
    {
        return PLOG_ERROR_OUT_OF_RANGE;
    }

    g_log_level = level;

    return PLOG_ERROR_OK;
}

plog_error_t
plog_appender_register(plog_appender_t appender, 
                       void* user_data, 
                       plog_appender_id_t* id)
{
    if (g_appender_count + 1 > PLOG_MAX_APPENDERS)
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
plog_appender_unregister(plog_appender_id_t id);

plog_error_t
plog_appender_enable(plog_appender_id_t id);

plog_error_t
plog_appender_disable(plog_appender_id_t id);

plog_error_t
plog_write(plog_level_t level, const char* p_fmt, ...);
