#ifndef PICOLOG_H
#define PICOLOG_H

#ifndef PICOLOG_MAX_APPENDERS
#   define PICOLOG_MAX_APPENDERS 16
#endif /* PICOLOG_MAX_APPENDERS */

typedef enum
{
    PICOLOG_LEVEL_DEBUG = 0,
    PICOLOG_LEVEL_INFO,
    PICOLOG_LEVEL_WARN,
    PICOLOG_LEVEL_ERROR,
    PICOLOG_LEVEL_FATAL,
    PICOLOG_LEVEL_COUNT
} picolog_level_t;

#define PICOLOG_DEBUG picolog_write(PICOLOG_LEVEL_DEBUG, FMT, ##__VAR_ARGS__)
#define PICOLOG_INFO  picolog_write(PICOLOG_LEVEL_INFO,  FMT, ##__VAR_ARGS__)
#define PICOLOG_INFO  picolog_write(PICOLOG_LEVEL_WARN,  FMT, ##__VAR_ARGS__)
#define PICOLOG_ERROR picolog_write(PICOLOG_LEVEL_ERROR, FMT, ##__VAR_ARGS__)
#define PICOLOG_FATAL picolog_write(PICOLOG_LEVEL_FATAL, FMT, ##__VAR_ARGS__)

typedef int (*picolog_appender_t)(const char* p_msg, void* p_user_data);
typedef size_t picolog_appender_id_t;

void
picolog_init();

int
picolog_set_level(picolog_level_t level);

int
picolog_set_user_data(void* p_user_data);

void*
picolog_get_user_data();

int
picolog_write(picolog_level_t level, const char* p_fmt, ...);

int
picolog_appender_register(picolog_appender_t appender, picolog_appender_id_t* id);

int
picolog_appender_unregister(picolog_appender_id_t id);

int
picolog_appender_set_user_data(picolog_appender_id_t id, void* p_user_data);

int
picolog_appender_get_user_data(picolog_appender_id_t id, void** pp_user_data);

int
picolog_appender_is_enabled(picolog_appender_id_t id);

int
picolog_appender_enable(picolog_appender_id_t id);

int
picolog_appender_disable(picolog_appender_id_t id);

#endif /* PICOLOG_H */

/* EoF */
