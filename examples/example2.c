#include <stdio.h>

#include "../picolog.h"

void appender1(const char* p_msg, void* p_user_data)
{
    printf("Appender 1: %s\n", p_msg);
}

void appender2(const char* p_msg, void* p_user_data)
{
    printf("Appender 2: %s\n", p_msg);
}

void log_all()
{
    PLOG_TRACE ("Test message: %d", 0);
    PLOG_DEBUG ("Test message: %d", 1);
    PLOG_INFO  ("Test message: %d", 2);
    PLOG_WARN  ("Test message: %d", 3);
    PLOG_ERROR ("Test message: %d", 4);
    PLOG_FATAL ("Test message: %d", 5);
}

int main(int argc, char** argv)
{
    plog_id_t id1, id2;

    plog_appender_register(appender1, NULL, &id1);
    plog_appender_register(appender2, NULL, &id2);

    plog_set_level(PLOG_LEVEL_TRACE);

    printf("================== Both appenders ==================\n");

    log_all();

    printf("================== One appender ==================\n");

    plog_appender_disable(id2);
    log_all();

    printf("================== Level Off ==================\n");

    plog_turn_level_off();
    log_all();

    printf("================== Level On/Set Level (INFO) ==================\n");

    plog_turn_level_on();
    plog_set_level(PLOG_LEVEL_INFO);
    log_all();

    printf("================== Timestamp ==================\n");

    plog_turn_timestamp_on();
    log_all();

    printf("================== File ==================\n");

    plog_turn_file_on();
    log_all();

    printf("================== Func ==================\n");

    plog_turn_func_on();
    log_all();


    return 0;
}
