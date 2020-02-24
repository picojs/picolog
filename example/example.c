#include <stdio.h>

#include "../picolog.h"

int appender1(const char* p_msg, void* p_user_data)
{
    printf("Appender 1: %s\n", p_msg);
    return 0;
}

int appender2(const char* p_msg, void* p_user_data)
{
    printf("Appender 2: %s\n", p_msg);
    return 0;
}

int main(int argc, char** argv)
{
    plog_appender_register(appender1, NULL, NULL);
    plog_appender_register(appender2, NULL, NULL);

    plog_enable();
    plog_timestamps_on();

    PLOG_INFO("test %d", 10);

    return 0;
}
