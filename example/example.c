#include <stdio.h>

#include "../picolog.h"

int appender1(const char* p_msg, void* p_user_data)
{
    printf("Appender 1: %s\n", p_msg);
    return 0;
}

int main(int argc, char** argv)
{
    plog_appender_register(appender1, NULL, NULL);
    plog_enable();

    PLOG_INFO("test %d", 10);

    return 0;
}
