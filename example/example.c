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

void log_all()
{
    PLOG_TRACE ("Test %d", 10);
    PLOG_DEBUG ("Test %d", 10);
    PLOG_INFO  ("Test %d", 10);
    PLOG_WARN  ("Test %d", 10);
    PLOG_ERROR ("Test %d", 10);
    PLOG_FATAL ("Test %d", 10);
}

int main(int argc, char** argv)
{
    plog_appender_id_t id1, id2;

    plog_appender_register(appender1, NULL, &id1);
    plog_appender_register(appender2, NULL, &id2);


    plog_set_level(PLOG_LEVEL_TRACE);

    printf("================== Both appenders ==================\n");

    log_all();

    printf("================== One appender ==================\n");

    plog_appender_disable(id2);
    log_all();

    printf("================== Level (INFO) ==================\n");

    plog_set_level(PLOG_LEVEL_INFO);
    log_all();

    printf("================== Timestamp ==================\n");

    plog_timestamp_on();
    log_all();

    printf("================== File ==================\n");


    plog_file_on();
    log_all();

    printf("================== Func ==================\n");


    plog_func_on();
    log_all();




    return 0;
}
