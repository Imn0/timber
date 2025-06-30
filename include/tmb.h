#ifndef TMB_H
#define TMB_H
#include <stdbool.h>

#ifndef _WIN32
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)                                  \
        __attribute__((format(printf, STR_IDX, ARG_BEGIN)))
#else
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)
#endif

extern const char* GIT_REV;
extern const char* GIT_TAG;
extern const char* GIT_BRANCH;
extern const char* GIT_PROJECT_VERSION;
extern const char* PROJECT_VERSION;
extern const char* SOVERSION;

#define TMB_LEVEL_TRACE 0
#define TMB_LEVEL_DEBUG 1
#define TMB_LEVEL_INFO 2
#define TMB_LEVEL_NOTICE 3
#define TMB_LEVEL_WARNING 4
#define TMB_LEVEL_ERROR 5
#define TMB_LEVEL_ALERT 6
#define TMB_LEVEL_CRITICAL 9

typedef enum {
    TMB_LOG_TRACE = TMB_LEVEL_TRACE,
    TMB_LOG_DEBUG = TMB_LEVEL_DEBUG,
    TMB_LOG_INFO = TMB_LEVEL_INFO,
    TMB_LOG_NOTICE = TMB_LEVEL_NOTICE,
    TMB_LOG_WARNING = TMB_LEVEL_WARNING,
    TMB_LOG_ERROR = TMB_LEVEL_ERROR,
    TMB_LOG_ALERT = TMB_LEVEL_ALERT,
    TMB_LOG_CRITICAL = TMB_LEVEL_CRITICAL,
} tmb_log_level;

typedef struct {
    const int line_no;
    const char* filename;
    const int filename_len;
    const char* funcname;
    const int funcname_len;
} LogCtx;

typedef struct {
    void** sinks;
    void** formatters;
} Logger;

#ifndef TMB_MIN_LOG_LEVEL
    #define TMB_MIN_LOG_LEVEL TMB_LEVEL_DEBUG
#endif

#if TMB_LEVEL_INFO >= TMB_MIN_LOG_LEVEL
    #define TMB_INFO(logger, ...) TMB_LOG(logger, TMB_LEVEL_INFO, __VA_ARGS__)
#else
    #define TMB_INFO(logger, ...)
#endif

#define TMB_LOG(logger, log_level, ...)                                        \
    __tmb_log(logger,                                                          \
              log_level,                                                       \
              (LogCtx) { __LINE__,                                             \
                         __FILE__,                                             \
                         sizeof(__FILE__),                                     \
                         __func__,                                             \
                         sizeof(__func__) },                                   \
              __VA_ARGS__)

bool tmb_init(const char* init_string);

void tmb_logger_init_default(Logger* lg);

void __tmb_log(const Logger* logger,
               tmb_log_level log_level,
               LogCtx ctx,
               const char* message,
               ...) TMB_FMT_CHECK(4, 5);

void tmb_test(void);

#endif
