/**
 * @brief Timber logging library. 
 * 
 * This file can be used as a normal header file for linking 
 * or as a STB style single header library.
 * 
 * When using as standalone header in one C file.
 * In one C file
 *  #define TMB_LOGGING_IMPLEMENTATION
 *  #include "tmb.h"
 * 
 * in other C files just include. 
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef TMB_H_
#define TMB_H_
#include <stdbool.h>
#include <time.h>

#if defined(_WIN32)
    #ifdef TMB_BUILD_DLL
        #define TMB_API __declspec(dllexport)
    // #elif defined(TMB_IMPORT_DLL)
    //     #define TMB_API __declspec(dllimport)
    #else
        #define TMB_API
    #endif
#else
    #define TMB_API
#endif

#if !defined(_MSC_VER) || defined(__clang__)
    // https://clang.llvm.org/docs/AttributeReference.html#format
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)                                  \
        __attribute__((format(printf, STR_IDX, ARG_BEGIN)))

    //https://clang.llvm.org/docs/AttributeReference.html#constructor-destructor
    #define TMB_INIT __attribute__((constructor))
    #define TMB_DEINIT __attribute__((destructor))
#else // not supported on MSVC
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)
    #define TMB_INIT
    #define TMB_DEINIT
#endif

static const char TMB_PATCH_V[] = "0";
static const char TMB_MINOR_V[] = "0";
static const char TMB_MAJOR_V[] = "0";
static const char TMB_SO_V[] = "0";
extern const char* const GIT_REV;

#define TMB_LEVEL_EMERGENCY 0
#define TMB_LEVEL_ALERT 1
#define TMB_LEVEL_CRITICAL 2
#define TMB_LEVEL_ERROR 3
#define TMB_LEVEL_WARNING 4
#define TMB_LEVEL_NOTICE 5
#define TMB_LEVEL_INFO 6
#define TMB_LEVEL_DEBUG 7

typedef enum {
    TMB_LOG_LEVEL_EMERGENCY = TMB_LEVEL_EMERGENCY,
    TMB_LOG_LEVEL_ALERT = TMB_LEVEL_ALERT,
    TMB_LOG_LEVEL_CRITICAL = TMB_LEVEL_CRITICAL,
    TMB_LOG_LEVEL_ERROR = TMB_LEVEL_ERROR,
    TMB_LOG_LEVEL_WARNING = TMB_LEVEL_WARNING,
    TMB_LOG_LEVEL_NOTICE = TMB_LEVEL_NOTICE,
    TMB_LOG_LEVEL_INFO = TMB_LEVEL_INFO,
    TMB_LOG_LEVEL_DEBUG = TMB_LEVEL_DEBUG,

    TMB_LOG_LEVEL_COUNT
} tmb_log_level;

extern const char* const tmb_log_level_str[TMB_LOG_LEVEL_COUNT];
extern int const tmb_log_level_str_len[TMB_LOG_LEVEL_COUNT];

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

/**
 * @brief Logger internal data, not to be edited manually.
 */
typedef struct {
    void** sinks;
    int sinks_count;
    void** formatters;
    int formatters_count;
    int* fmt_map;
} Logger;

/**
 * @brief Log context meant to be created with macros.
 * 
 */
typedef struct {
    const tmb_log_level log_level;
    const int line_no;
    const char* const filename;
    const int filename_len;
    const char* const funcname;
    const int funcname_len;
    const time_t log_time;
} LogCtx;

/**
 * @brief Initializes default logger.
 * 
 * on gcc and clang has constructor attribute.
 * 
 */
TMB_API TMB_INIT void tmb_init(void);

/**
 * @brief Deinitializes default logger.
 * 
 * on gcc and clang has destructor attribute.
 * 
 */
TMB_API TMB_DEINIT void tmb_deinit(void);

/**
 * @brief Initializes logger with config string.
 * 
 * @param lgr Logger to initialize
 * @param config conifg string.
 * @return true initialization succeeded
 * @return false initialization failed
 */
TMB_API bool tmb_logger_init(Logger* lgr, const char* config);

/**
 * @brief Initializes logger with configuration from file.
 * 
 * @param lgr Logger to initialize
 * @param filename config file
 * @return true initialization succeeded
 * @return false initialization failed
 */
TMB_API bool tmb_logger_init_file(Logger* lgr, const char* filename);

/**
 * @brief Initializes logger with default configuration.
 * 
 * @param lgr Logger to initialize
 * @return true initialization succeeded
 * @return false initialization failed
 */
TMB_API bool tmb_logger_init_default(Logger* lgr);

/**
 * @brief Deinitializes logger freeing internal managed memory.
 *  Does not free the logger itself.
 * 
 * @param lgr Logger to deinitialize
 * @return true deinitialization succeeded
 * @return false deinitialization failed
 */
TMB_API bool tmb_logger_deinit(Logger* lgr);

/**
 * @brief Loggs message with given logger and context.
 * 
 * @param ctx 
 * @param logger 
 * @param message string in printf format. 
 * @param ... 
 */
TMB_API void tmb_log(LogCtx ctx, const Logger* logger, const char* message, ...)
        TMB_FMT_CHECK(3, 4);

/**
 * @brief Loggs message with context and default logger.
 * 
 * @param ctx 
 * @param logger 
 * @param message string in printf format. 
 * @param ... 
 */
TMB_API void tmb_log_default(LogCtx ctx, const char* message, ...)
        TMB_FMT_CHECK(2, 3);

/**
 * @brief prints version string to stdout
 * 
 */
TMB_API void tmb_print_version(void);

#ifndef TMB_MIN_LOG_LEVEL
    #define TMB_MIN_LOG_LEVEL TMB_LEVEL_INFO
#endif

#define TMB_CALL(func, ...) func(__VA_ARGS__)
#define TMB_CONST_STR_SIZE(X) (sizeof(X) - 1)

// clang-format off
#if TMB_MIN_LOG_LEVEL > TMB_LEVEL_DEBUG || TMB_MIN_LOG_LEVEL < TMB_LEVEL_EMERGENCY
    #error "TMB_MIN_LOG_LEVEL must be between TMB_LEVEL_DEBUG(7) and TMB_LEVEL_EMERGENCY(0)"
#endif

#if TMB_LEVEL_EMERGENCY <= TMB_MIN_LOG_LEVEL
    #define TMB_EMERGENCY(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_EMERGENCY, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_EMERGENCY(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_ALERT <= TMB_MIN_LOG_LEVEL
    #define TMB_ALERT(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_ALERT, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_ALERT(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_CRITICAL <= TMB_MIN_LOG_LEVEL
    #define TMB_CRITICAL(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_CRITICAL, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_CRITICAL(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_ERROR <= TMB_MIN_LOG_LEVEL
    #define TMB_ERROR(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_ERROR, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_ERROR(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_WARNING <= TMB_MIN_LOG_LEVEL
    #define TMB_WARNING(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_WARNING, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_WARNING(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_NOTICE <= TMB_MIN_LOG_LEVEL
    #define TMB_NOTICE(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_NOTICE, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_NOTICE(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_INFO <= TMB_MIN_LOG_LEVEL
    #define TMB_INFO(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_INFO, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_INFO(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_DEBUG <= TMB_MIN_LOG_LEVEL
    #define TMB_DEBUG(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_DEBUG, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_DEBUG(lgr_or_fmt, ...)
#endif
// clang-format on

#define TMB_DISPATCH(ctx, logger_or_format, ...)                               \
    TMB_CALL(_Generic((logger_or_format),                                      \
                     Logger*: tmb_log,                                         \
                     char*: tmb_log_default),                                  \
             ctx,                                                              \
             logger_or_format __VA_OPT__(, ) __VA_ARGS__)

#define TMB_LOG(log_level, logger_or_format, ...)                              \
    do {                                                                       \
        time_t now;                                                            \
        time(&now);                                                            \
        LogCtx _m__ctx = { log_level, __LINE__,                                \
                           __FILE__,  TMB_CONST_STR_SIZE(__FILE__),            \
                           __func__,  TMB_CONST_STR_SIZE(__func__),            \
                           now };                                              \
        TMB_DISPATCH(_m__ctx, logger_or_format __VA_OPT__(, ) __VA_ARGS__);    \
    } while (0)

#ifdef TMB_LOGGING_IMPLEMENTATION
    /**
 * Implementation for header only mode
 * see https://github.com/nothings/stb
 */

    #include <stdarg.h>
    #include <stdio.h>

const char* const tmb_log_level_str[] = {
    [TMB_LOG_LEVEL_EMERGENCY] = "EMERGENCY", [TMB_LOG_LEVEL_ALERT] = "ALERT",
    [TMB_LOG_LEVEL_CRITICAL] = "CRITICAL",   [TMB_LOG_LEVEL_ERROR] = "ERROR",
    [TMB_LOG_LEVEL_WARNING] = "WARNING",     [TMB_LOG_LEVEL_NOTICE] = "NOTICE",
    [TMB_LOG_LEVEL_INFO] = "INFO",           [TMB_LOG_LEVEL_DEBUG] = "DEBUG",
};

const int tmb_log_level_str_len[] = {
    [TMB_LOG_LEVEL_EMERGENCY] = TMB_CONST_STR_SIZE("EMERGENCY"),
    [TMB_LOG_LEVEL_ALERT] = TMB_CONST_STR_SIZE("ALERT"),
    [TMB_LOG_LEVEL_CRITICAL] = TMB_CONST_STR_SIZE("CRITICAL"),
    [TMB_LOG_LEVEL_ERROR] = TMB_CONST_STR_SIZE("ERROR"),
    [TMB_LOG_LEVEL_WARNING] = TMB_CONST_STR_SIZE("WARNING"),
    [TMB_LOG_LEVEL_NOTICE] = TMB_CONST_STR_SIZE("NOTICE"),
    [TMB_LOG_LEVEL_INFO] = TMB_CONST_STR_SIZE("INFO"),
    [TMB_LOG_LEVEL_DEBUG] = TMB_CONST_STR_SIZE("DEBUG"),
};

TMB_INIT void tmb_init() {}
TMB_DEINIT void tmb_deinit() {}
bool tmb_logger_init(Logger* lgr, const char* config) {
    return true;
}
bool tmb_logger_init_file(Logger* lgr, const char* filename) {
    return true;
}
bool tmb_logger_init_default(Logger* lgr) {
    return true;
}
bool tmb_logger_deinit(Logger* lgr) {
    return true;
}

void tmb_log_imp(LogCtx ctx, const char* message, va_list args) {
    fprintf(stderr, "[ %s ] ", tmb_log_level_str[ctx.log_level]);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
}

void tmb_log(LogCtx ctx, const Logger* logger, const char* message, ...) {
    va_list args;
    va_start(args, message);
    tmb_log_imp(ctx, message, args);
    va_end(args);
}

void tmb_log_default(LogCtx ctx, const char* message, ...) {
    va_list args;
    va_start(args, message);
    tmb_log_imp(ctx, message, args);
    va_end(args);
}

void tmb_print_version(void) {
    printf("%s.%s.%s @ HEADER IMPL\nSO Version: %s\n",
           TMB_PATCH_V,
           TMB_MINOR_V,
           TMB_MAJOR_V,
           TMB_SO_V);
}
#endif // TMB_LOGGING_IMPLEMENTATION

#endif // TMB_H_
