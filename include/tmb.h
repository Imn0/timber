#ifndef TMB_H
#define TMB_H
#include <stdbool.h>

#ifndef _WIN32
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)                                  \
        __attribute__((format(printf, STR_IDX, ARG_BEGIN)))
#else
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)
#endif

static const char TMB_PATCH_V[] = "0";
static const char TMB_MINOR_V[] = "0";
static const char TMB_MAJOR_V[] = "0";
static const char TMB_SO_V[] = "0";
extern const char* GIT_REV;

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
    const int log_level;
    const int line_no;
    const char* filename;
    const int filename_len;
    const char* funcname;
    const int funcname_len;
} LogCtx;

typedef struct {
    void** sinks;
    int sinks_count;
    void** formatters;
    int formatters_count;
    int* fmt_map;
} Logger;

#ifndef TMB_MIN_LOG_LEVEL
    #define TMB_MIN_LOG_LEVEL TMB_LEVEL_DEBUG
#endif

// clang-format off
#if TMB_LEVEL_TRACE >= TMB_MIN_LOG_LEVEL
    #define TMB_TRACE(logger, ...) TMB_LOG(logger, TMB_LEVEL_TRACE, __VA_ARGS__)
#else
    #define TMB_TRACE(logger, ...)
#endif

#if TMB_LEVEL_DEBUG >= TMB_MIN_LOG_LEVEL
    #define TMB_DEBUG(logger, ...) TMB_LOG(logger, TMB_LEVEL_DEBUG, __VA_ARGS__)
#else
    #define TMB_DEBUG(logger, ...)
#endif

#if TMB_LEVEL_INFO >= TMB_MIN_LOG_LEVEL
    #define TMB_INFO(logger, ...) TMB_LOG(logger, TMB_LEVEL_INFO, __VA_ARGS__)
#else
    #define TMB_INFO(logger, ...)
#endif

#if TMB_LEVEL_NOTICE >= TMB_MIN_LOG_LEVEL
    #define TMB_NOTICE(logger, ...) TMB_LOG(logger, TMB_LEVEL_NOTICE, __VA_ARGS__)
#else
    #define TMB_NOTICE(logger, ...)
#endif

#if TMB_LEVEL_WARNING >= TMB_MIN_LOG_LEVEL
    #define TMB_WARNING(logger, ...) TMB_LOG(logger, TMB_LEVEL_WARNING, __VA_ARGS__)
#else
    #define TMB_WARNING(logger, ...)
#endif

#if TMB_LEVEL_ERROR >= TMB_MIN_LOG_LEVEL
    #define TMB_ERROR(logger, ...) TMB_LOG(logger, TMB_LEVEL_ERROR, __VA_ARGS__)
#else
    #define TMB_ERROR(logger, ...)
#endif

#if TMB_LEVEL_ALERT >= TMB_MIN_LOG_LEVEL
    #define TMB_ALERT(logger, ...) TMB_LOG(logger, TMB_LEVEL_ALERT, __VA_ARGS__)
#else
    #define TMB_ALERT(logger, ...)
#endif

#if TMB_LEVEL_CRITICAL >= TMB_MIN_LOG_LEVEL
    #define TMB_CRITICAL(logger, ...) TMB_LOG(logger, TMB_LEVEL_CRITICAL, __VA_ARGS__)
#else
    #define TMB_CRITICAL(logger, ...)
#endif
// clang-format on

#define TMB_LOG(logger, log_level, ...)                                        \
    tmb_log(logger,                                                            \
            (LogCtx) { log_level,                                              \
                       __LINE__,                                               \
                       __FILE__,                                               \
                       sizeof(__FILE__),                                       \
                       __func__,                                               \
                       sizeof(__func__) },                                     \
            __VA_ARGS__)

bool tmb_logger_init_default(Logger* lg);
bool tmb_logger_destroy(Logger* lg);

void tmb_log(const Logger* logger, LogCtx ctx, const char* message, ...)
        TMB_FMT_CHECK(3, 4);

void tmb_test(void);

#endif
