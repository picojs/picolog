#include <stdio.h>

#include "../picolog.h"

void appender(const char* p_msg, void* p_user_data)
{
    printf("%s\n", p_msg);
}

int main(int argc, char** argv)
{
    plog_appender_register(appender, NULL);

    plog_turn_timestamp_on();
    plog_turn_file_on();
    plog_turn_func_on();

    PLOG_TRACE ("Test message: %d", 0);
    PLOG_DEBUG ("Test message: %d", 1);
    PLOG_INFO  ("Test message: %d", 2);
    PLOG_WARN  ("Test message: %d", 3);
    PLOG_ERROR ("Test message: %d", 4);
    PLOG_FATAL ("Test message: %d", 5);

    return 0;
}
