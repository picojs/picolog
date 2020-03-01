/*=============================================================================
 * MIT License
 *
 * Copyright (c) 2020 James McLean
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
=============================================================================*/

#include <picolog.h>

#include <stdio.h>

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
    plog_appender_register(appender1, NULL);

    plog_id_t id = plog_appender_register(appender2, NULL);

    plog_set_level(PLOG_LEVEL_TRACE);

    printf("================== Both appenders ==================\n");

    log_all();

    printf("================== One appender ==================\n");

    plog_appender_disable(id);
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
