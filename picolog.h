#ifndef PICOLOG_H
#define PICOLOG_H

#include <stdbool.h> /* bool, true, false */
#include <stddef.h>  /* NULL, size_t */

#ifndef PLOG_MAX_APPENDERS
#   define PLOG_MAX_APPENDERS 16
#endif /* PLOG_MAX_APPENDERS */

typedef enum
{
    PLOG_LEVEL_DEBUG = 0,
    PLOG_LEVEL_INFO,
    PLOG_LEVEL_WARN,
    PLOG_LEVEL_ERROR,
    PLOG_LEVEL_FATAL,
    PLOG_LEVEL_COUNT
} plog_level_t;

#define PLOG_DEBUG(FMT, ...) \
        plog_write(PLOG_LEVEL_DEBUG, FMT, ##__VAR_ARGS__)

#define PLOG_INFO (FMT, ...) \
        plog_write(PLOG_LEVEL_INFO,  FMT, ##__VAR_ARGS__)

#define PLOG_WARN (FMT, ...) \
         plog_write(PLOG_LEVEL_WARN,  FMT, ##__VAR_ARGS__)

#define PLOG_ERROR(FMT, ...) \
        plog_write(PLOG_LEVEL_ERROR, FMT, ##__VAR_ARGS__)

#define PLOG_FATAL(FMT, ...) \
        plog_write(PLOG_LEVEL_FATAL, FMT, ##__VAR_ARGS__)

typedef int (*plog_appender_t)(const char* p_msg, void* p_user_data);
typedef size_t plog_appender_id_t;

typedef enum
{
    PLOG_ERROR_OK           =  0,
    PLOG_ERROR_OUT_OF_RANGE = -1,
    PLOG_ERROR_OUT_OF_SPACE = -2,
    PLOG_ERROR_INVALD_ARG   = -3,
    PLOG_ERROR_INVALD_ID    = -4,
    PLOG_ERROR_UNKNOWN      = -5 /* This should never happen */
} plog_error_t;

void
plog_init();

void
plog_enable();

void
plog_disable();

plog_error_t
plog_set_level(plog_level_t level);

plog_error_t
plog_appender_register(plog_appender_t appender,
                       void* p_user_data, 
                       plog_appender_id_t* id);

plog_error_t
plog_appender_unregister(plog_appender_id_t id);

plog_error_t
plog_appender_enable(plog_appender_id_t id);

plog_error_t
plog_appender_disable(plog_appender_id_t id);

plog_error_t
plog_write(plog_level_t level, const char* p_fmt, ...);

#endif /* PICOLOG_H */

/* EoF */
