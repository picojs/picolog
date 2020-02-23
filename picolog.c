#include "picolog.h"

typedef struct
{
    picolog_appender_t p_appender;
    void*              p_user_data;
    bool               b_enabled;
} appender_info_t;

static appender_info_t gp_appenders[PICOLOG_MAX_APPENDERS];

static void*  gp_user_data = NULL;
static size_t g_appender_count = 0;
static int    g_log_level  = PICOLOG_LEVEL_DEBUG;

void picolog_init()
{
    g_log_level = PICOLOG_LEVEL_DEBUG;

    for (int i = 0; i < PICOLOG_MAX_APPENDERS; i++)
    {
        gp_appenders[i].p_appender  = NULL;
        gp_appenders[i].p_user_data = NULL;
        gp_appenders[i].b_enabled   = false;
    }
}

picolog_error_t
picolog_set_level(picolog_level_t level)
{
    if (level < 0 || level >= PICOLOG_LEVEL_COUNT)
    {
        return PICOLOG_ERROR_OUT_OF_RANGE;
    }
    
    g_log_level = level;

    return PICOLOG_ERROR_OK;
}

void
picolog_set_user_data(void* p_user_data)
{
    gp_user_data = p_user_data;
}   

void*
picolog_get_user_data()
{
    return gp_user_data;
}

picolog_error_t
picolog_appender_register(picolog_appender_t appender, picolog_appender_id_t* id)
{
    if (g_appender_count + 1 > PICOLOG_MAX_APPENDERS)
    {
        return PICOLOG_ERROR_OUT_OF_SPACE;
    }

    for (int i = 0; i < PICOLOG_MAX_APPENDERS; i++)
    {
        if (NULL == gp_appenders[i].p_appender)
        {
            gp_appenders[i].p_appender = appender;
            
            if (NULL != id)
            {
                *id = i;
            }           
            
            g_appender_count++;
            
            return PICOLOG_ERROR_OK;
        }
    }    
    
    return PICOLOG_ERROR_UNKNOWN;
}

picolog_error_t
picolog_appender_unregister(picolog_appender_id_t id);

picolog_error_t
picolog_appender_set_user_data(picolog_appender_id_t id, void* p_user_data);

picolog_error_t
picolog_appender_get_user_data(picolog_appender_id_t id, void** pp_user_data);

picolog_error_t
picolog_appender_is_enabled(picolog_appender_id_t id);

picolog_error_t
picolog_appender_enable(picolog_appender_id_t id);

picolog_error_t
picolog_appender_disable(picolog_appender_id_t id);

picolog_error_t
picolog_write(picolog_level_t level, const char* p_fmt, ...);
