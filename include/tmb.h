#ifndef TMB_H
#define TMB_H
#include <stdbool.h>
#include <time.h>

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
extern const char* const GIT_REV;

#define TMB_LEVEL_TRACE 0
#define TMB_LEVEL_DEBUG 1
#define TMB_LEVEL_INFO 2
#define TMB_LEVEL_NOTICE 3
#define TMB_LEVEL_WARNING 4
#define TMB_LEVEL_ERROR 5
#define TMB_LEVEL_ALERT 6
#define TMB_LEVEL_CRITICAL 7

typedef enum {
    TMB_LOG_TRACE = TMB_LEVEL_TRACE,
    TMB_LOG_DEBUG = TMB_LEVEL_DEBUG,
    TMB_LOG_INFO = TMB_LEVEL_INFO,
    TMB_LOG_NOTICE = TMB_LEVEL_NOTICE,
    TMB_LOG_WARNING = TMB_LEVEL_WARNING,
    TMB_LOG_ERROR = TMB_LEVEL_ERROR,
    TMB_LOG_ALERT = TMB_LEVEL_ALERT,
    TMB_LOG_CRITICAL = TMB_LEVEL_CRITICAL,

    TMB_LOG_COUNT
} tmb_log_level;

extern const char* const tmb_log_level_str[TMB_LOG_COUNT];
extern int const tmb_log_level_str_len[TMB_LOG_COUNT];

#define ANSI_ESCAPE "\x1b["
#define ANSI_BLUE ANSI_ESCAPE "34m"
#define ANSI_RESET ANSI_ESCAPE "0m"

#define ANSI_BLACK ANSI_ESCAPE "30m"
#define ANSI_RED ANSI_ESCAPE "31m"
#define ANSI_GREEN ANSI_ESCAPE "32m"
#define ANSI_YELLOW ANSI_ESCAPE "33m"
#define ANSI_BLUE ANSI_ESCAPE "34m"
#define ANSI_MAGENTA ANSI_ESCAPE "35m"
#define ANSI_CYAN ANSI_ESCAPE "36m"
#define ANSI_WHITE ANSI_ESCAPE "37m"

typedef struct {
    void** sinks;
    int sinks_count;
    void** formatters;
    int formatters_count;
    int* fmt_map;
} Logger;

#ifndef TMB_MIN_LOG_LEVEL
    #define TMB_MIN_LOG_LEVEL TMB_LEVEL_INFO
#endif

// clang-format off
#if TMB_MIN_LOG_LEVEL < TMB_LEVEL_TRACE || TMB_MIN_LOG_LEVEL > TMB_LEVEL_CRITICAL
    #error "TMB_MIN_LOG_LEVEL must be between TMB_LEVEL_TRACE(9) and TMB_LEVEL_CRITICAL(9)"
#endif

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

typedef struct {
    const tmb_log_level log_level;
    const int line_no;
    const char* const filename;
    const int filename_len;
    const char* const funcname;
    const int funcname_len;
    const time_t log_time;
} LogCtx;

#define TMB_LOG(logger, log_level, ...)                                        \
    do {                                                                       \
        time_t now;                                                            \
        time(&now);                                                            \
        LogCtx _m__ctx = { log_level, __LINE__,                                \
                           __FILE__,  sizeof(__FILE__) - 1,                    \
                           __func__,  sizeof(__func__) - 1,                    \
                           now }; /* -1 coz sizeof includes null terminator */ \
        tmb_log(logger, _m__ctx, __VA_ARGS__);                                 \
    } while (0)

bool tmb_logger_init(Logger* lgr, const char* config);
bool tmb_logger_init_file(Logger* lgr, const char* filename);
bool tmb_logger_init_default(Logger* lgr);
bool tmb_logger_destroy(Logger* lgr);

void tmb_log(const Logger* logger, LogCtx ctx, const char* message, ...)
        TMB_FMT_CHECK(3, 4);

void tmb_print_version(void);

#endif
