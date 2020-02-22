#ifndef pico_log_H
#define pico_log_H



#ifndef PICO_LOG_MAX_APPENDERS
#   define PICO_LOG_MAX_APPENDERS 16
#endif /* PICO_LOG_MAX_APPENDERS */

typedef enum
{
    PICO_LOG_LEVEL_DEBUG = 0,
    PICO_LOG_LEVEL_INFO,
    PICO_LOG_LEVEL_WARN,
    PICO_LOG_LEVEL_ERROR,
    PICO_LOG_LEVEL_FATAL,
    PICO_LOG_LEVEL_COUNT
} pico_log_level_t;

#define PICO_LOG_DEBUG pico_log_write(PICO_LOG_LEVEL_DEBUG, FMT, ##__VAR_ARGS__)
#define PICO_LOG_INFO  pico_log_write(PICO_LOG_LEVEL_INFO,  FMT, ##__VAR_ARGS__)
#define PICO_LOG_INFO  pico_log_write(PICO_LOG_LEVEL_WARN,  FMT, ##__VAR_ARGS__)
#define PICO_LOG_ERROR pico_log_write(PICO_LOG_LEVEL_ERROR, FMT, ##__VAR_ARGS__)
#define PICO_LOG_FATAL pico_log_write(PICO_LOG_LEVEL_FATAL, FMT, ##__VAR_ARGS__)

typedef int (pico_log_app_t*)(const char* p_msg, void* p_user_data);
typedef size_t pico_log_app_id_t;

typedef enum
{
    PICO_LOG_ERROR_NONE,
    PICO_LOG_ERROR_INVALID_ID,
    PICO_LOG_ERROR_COUNT,
} pico_log_error_id_t;

void pico_log_init();

pico_log_error_id_t pico_log_error_get_id();

const char*
pico_log_error_get_str(pico_log_error_id_t id);

int
pico_log_set_level(pico_log_level_t level);

int
pico_log_set_user_data(void* p_user_data);

void*
pico_log_get_user_data();

int
pico_log_write(pico_log_level_t level, const char* p_fmt, ...);

int
pico_log_app_register(pico_log_app_t appender, pico_log_app_id_t* id);

int
pico_log_app_unregister(pico_log_app_id_t id);

int
pico_log_app_set_user_data(pico_log_app_id_t id, void* p_user_data);

int
pico_log_app_get_user_data(pico_log_app_id_t id, void** p_user_data);

int
pico_log_app_is_enabled(pico_log_app_id_t id);

int
pico_log_app_enable(pico_log_app_id_t id);

int
pico_log_app_disable(pico_log_app_id_t id);

#endif /* pico_log_H */

/* EoF */
