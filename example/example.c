#include <stdio.h>

#include "../picolog.h"

bool appender1(const char* p_msg, void* p_user_data)
{
    printf("Appender 1: %s\n", p_msg);
    return true;
}

bool appender2(const char* p_msg, void* p_user_data)
{
    printf("Appender 2: %s\n", p_msg);
    return true;
}

int main(int argc, char** argv)
{
    plog_appender_register(appender1, NULL, NULL);
    plog_appender_register(appender2, NULL, NULL);

    plog_enable();
    plog_timestamp_on();
    plog_file_on();
    plog_func_on();

    PLOG_INFO("test %d", 10);

    return 0;
}
