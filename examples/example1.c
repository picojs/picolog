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
