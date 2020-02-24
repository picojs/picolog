#ifndef PICOLOG_H
#define PICOLOG_H

#include <stdarg.h>
#include <stddef.h> /* NULL, size_t */

#ifndef PLOG_MAX_APPENDERS
#   define PLOG_MAX_APPENDERS 8
#endif /* PLOG_MAX_APPENDERS */

#ifndef PLOG_MAX_MSG_LENGTH
#   define PLOG_MAX_MSG_LENGTH 1024
#endif /* PLOG_MAX_MSG_LENGTH */

typedef enum
{
    PLOG_LEVEL_DEBUG = 0,
    PLOG_LEVEL_INFO,
    PLOG_LEVEL_WARN,
    PLOG_LEVEL_ERROR,
    PLOG_LEVEL_FATAL,
    PLOG_LEVEL_COUNT
} plog_level_t;

typedef enum
{
    PLOG_ERROR_OK = 0,
    PLOG_ERROR_MAX_APPENDERS,
    PLOG_ERROR_INVALD_ARG,
    PLOG_ERROR_INVALD_ID,
    PLOG_ERROR_NOT_ENABLED,
    PLOG_ERROR_UNKNOWN, /* This should never happen */
    PLOG_ERROR_COUNT,
} plog_error_t;

#define PLOG_DEBUG(...) \
        plog_write(PLOG_LEVEL_DEBUG, __VA_ARGS__)

#define PLOG_INFO(...) \
        plog_write(PLOG_LEVEL_INFO,  __VA_ARGS__)

#define PLOG_WARN(...) \
        plog_write(PLOG_LEVEL_WARN,  __VA_ARGS__)

#define PLOG_ERROR(...) \
        plog_write(PLOG_LEVEL_ERROR, __VA_ARGS__)

#define PLOG_FATAL(...) \
        plog_write(PLOG_LEVEL_FATAL, __VA_ARGS__)

typedef int   (*plog_appender_t)(const char* p_msg, void* p_user_data);
typedef size_t  plog_appender_id_t;

const char*
plog_error_str(plog_error_t error_code);


plog_error_t plog_set_level(plog_level_t level);

plog_error_t plog_appender_register(plog_appender_t appender,
                                    void* p_user_data,
                                    plog_appender_id_t* id);

plog_error_t plog_appender_unregister(plog_appender_id_t id);

plog_error_t plog_appender_enable(plog_appender_id_t id);

plog_error_t plog_appender_disable(plog_appender_id_t id);

void plog_enable();

void plog_disable();

plog_error_t plog_write(plog_level_t level, const char* p_fmt, ...);

#endif /* PICOLOG_H */

/* EoF */
