#include "picologger.h"

#include <stddef.h>  /* NULL */

typedef struct
{
    pico_appender_t p_appender;
    void* p_user_data;
    int enabled;
} app_info_t;

static app_holder_t gp_apps[PICO_LOG_MAX_APPENDERS];
static void* gp_user_data = NULL;

void pico_log_init()
{
    int i;

    for (i = 0; i < PICO_LOG_MAX_APPENDERS; i++)
    {
        gp_apps[i]->p_appender = NULL;
        gp_apps[i]->p_user_data = NULL;
        gp_apps[i]->enabled = 0;
    }
}
