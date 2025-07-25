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

#if defined(_WIN32) && defined(_MSC_VER)
    #ifdef TMB_WIN_USE_STAIC
        #define TMB_API
    #elif defined(TMB_BUILD_DLL)
        #define TMB_API __declspec(dllexport)
    #else
        #define TMB_API __declspec(dllimport)
    #endif
#else
    #ifdef TMB_BUILD_DLL
        #define TMB_API __attribute__((visibility("default")))
    #else
        #define TMB_API
    #endif
#endif

#if !defined(_MSC_VER) || defined(__clang__)
    // https://clang.llvm.org/docs/AttributeReference.html#format
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)                                  \
        __attribute__((format(printf, STR_IDX, ARG_BEGIN)))

    //https://clang.llvm.org/docs/AttributeReference.html#constructor-destructor
    #define TMB_INIT   __attribute__((constructor))
    #define TMB_DEINIT __attribute__((destructor))
#else // not supported on MSVC
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)
    #define TMB_INIT
    #define TMB_DEINIT
#endif

static const char TMB_PATCH_V[] = "0";
static const char TMB_MINOR_V[] = "0";
static const char TMB_MAJOR_V[] = "0";
static const char TMB_SO_V[]    = "0";
extern const char* const GIT_REV;

#define TMB_LEVEL_NONE    -1
#define TMB_LEVEL_FATAL   0
#define TMB_LEVEL_ERROR   1
#define TMB_LEVEL_WARNING 2
#define TMB_LEVEL_INFO    3
#define TMB_LEVEL_DEBUG   4
#define TMB_LEVEL_TRACE   5
#define TMB_LEVEL_ALL     5

typedef enum {
    TMB_LOG_LEVEL_FATAL   = TMB_LEVEL_FATAL,
    TMB_LOG_LEVEL_ERROR   = TMB_LEVEL_ERROR,
    TMB_LOG_LEVEL_WARNING = TMB_LEVEL_WARNING,
    TMB_LOG_LEVEL_INFO    = TMB_LEVEL_INFO,
    TMB_LOG_LEVEL_DEBUG   = TMB_LEVEL_DEBUG,
    TMB_LOG_LEVEL_TRACE   = TMB_LEVEL_TRACE,

    TMB_LOG_LEVEL_COUNT
} tmb_log_level;

extern const char* const tmb_log_level_str[TMB_LOG_LEVEL_COUNT];
extern int const tmb_log_level_str_len[TMB_LOG_LEVEL_COUNT];

#define ANSI_RESET     "0"
#define ANSI_BOLD      "1"
#define ANSI_DIM       "2"
#define ANSI_ITALIC    "3"
#define ANSI_UNDERLINE "4"
#define ANSI_BLINK     "5"
#define ANSI_REVERSE   "7"

#define ANSI_BLACK   "30"
#define ANSI_RED     "31"
#define ANSI_GREEN   "32"
#define ANSI_YELLOW  "33"
#define ANSI_BLUE    "34"
#define ANSI_MAGENTA "35"
#define ANSI_CYAN    "36"
#define ANSI_WHITE   "37"

#define ANSI_BG_BLACK   "40"
#define ANSI_BG_RED     "41"
#define ANSI_BG_GREEN   "42"
#define ANSI_BG_YELLOW  "43"
#define ANSI_BG_BLUE    "44"
#define ANSI_BG_MAGENTA "45"
#define ANSI_BG_CYAN    "46"
#define ANSI_BG_WHITE   "47"

#define ANSI_256(ID)    "38;5;" ID
#define ANSI_BG_256(ID) "48;5;" ID

#define MAKE_ANSI(...)                                                         \
    MAKE_ANSI_SELECT(__VA_ARGS__,                                              \
                     MAKE_ANSI6,                                               \
                     MAKE_ANSI5,                                               \
                     MAKE_ANSI4,                                               \
                     MAKE_ANSI3,                                               \
                     MAKE_ANSI2,                                               \
                     MAKE_ANSI1)(__VA_ARGS__)
#define MAKE_ANSI_SELECT(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define MAKE_ANSI1(a)                                       "\x1b[" a "m"
#define MAKE_ANSI2(a, b)                                    "\x1b[" a ";" b "m"
#define MAKE_ANSI3(a, b, c)                                 "\x1b[" a ";" b ";" c "m"
#define MAKE_ANSI4(a, b, c, d)                              "\x1b[" a ";" b ";" c ";" d "m"
#define MAKE_ANSI5(a, b, c, d, e)                           "\x1b[" a ";" b ";" c ";" d ";" e "m"
#define MAKE_ANSI6(a, b, c, d, e, f)                                           \
    "\x1b[" a ";" b ";" c ";" d ";" e ";" f "; "                               \
    "m"

#define _da_header_(T)                                                         \
    size_t size;                                                               \
    size_t capacity;                                                           \
    T items

typedef struct {
    _da_header_(char*);
} StringBuilder;

typedef struct {
    size_t size;
    char* items;
} String;

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

typedef void fmt_fn_t(StringBuilder* sb, const LogCtx* ctx, void* data);
typedef void sink_log_fn_t(const String* msg, void* data);
typedef void free_fn_t(void* data);

// sink definition
typedef enum { STDERR_SINK } SINK_TYPE;

typedef struct {
    sink_log_fn_t* sink_log;
    free_fn_t* free_fn;
    void* sink_data;
} Sink;
// sink definition end

typedef enum { FMT_FN, FMT_MSG } FormatTokenType;

typedef struct {
    FormatTokenType type;
    void* token_data;
    fmt_fn_t* fmt_function;
    free_fn_t* free_fn;
} FormatToken;

typedef struct {
    _da_header_(FormatToken*);
} Formatter;

/**
 * @brief Logger internal data, not to be edited manually.
 */
typedef struct {
    struct {
        _da_header_(Sink*);
    } sinks;
    struct {
        _da_header_(Formatter*);
    } formatters;
    struct {
        _da_header_(size_t*);
    } fmt_map;
} Logger;

struct Logger;

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

#define TMB_CALL(func, ...)   func(__VA_ARGS__)
#define TMB_CONST_STR_SIZE(X) (sizeof(X) - 1)

// clang-format off
#if TMB_LEVEL_FATAL <= TMB_MIN_LOG_LEVEL
    #define LOG_FATAL(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_FATAL, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define LOG_FATAL(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_ERROR <= TMB_MIN_LOG_LEVEL
    #define LOG_ERROR(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_ERROR, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define LOG_ERROR(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_WARNING <= TMB_MIN_LOG_LEVEL
    #define LOG_WARNING(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_WARNING, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
    #define LOG_WARN(lgr_or_fmt, ...) LOG_WARNING(lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define LOG_WARN(lgr_or_fmt, ...)
    #define LOG_WARNING(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_INFO <= TMB_MIN_LOG_LEVEL
    #define LOG_INFO(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_INFO, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define LOG_INFO(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_DEBUG <= TMB_MIN_LOG_LEVEL
    #define LOG_DEBUG(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_DEBUG, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define LOG_DEBUG(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_TRACE <= TMB_MIN_LOG_LEVEL
    #define LOG_TRACE(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_TRACE, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define LOG_TRACE(lgr_or_fmt, ...)
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
    [TMB_LEVEL_FATAL] = "FATAL",     [TMB_LEVEL_ERROR] = "ERROR",
    [TMB_LEVEL_WARNING] = "WARNING", [TMB_LEVEL_INFO] = "INFO",
    [TMB_LEVEL_DEBUG] = "DEBUG",     [TMB_LEVEL_TRACE] = "TRACE",
};

const int tmb_log_level_str_len[] = {
    [TMB_LEVEL_FATAL]   = TMB_CONST_STR_SIZE("FATAL"),
    [TMB_LEVEL_ERROR]   = TMB_CONST_STR_SIZE("ERROR"),
    [TMB_LEVEL_WARNING] = TMB_CONST_STR_SIZE("WARNING"),
    [TMB_LEVEL_INFO]    = TMB_CONST_STR_SIZE("INFO"),
    [TMB_LEVEL_DEBUG]   = TMB_CONST_STR_SIZE("DEBUG"),
    [TMB_LEVEL_TRACE]   = TMB_CONST_STR_SIZE("TRACE"),
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
