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
 * In header only mode there is only one logger, one format, one sink.
 * All of the functions technically work and wont cause program to crash,
 * however its output will be different
 * in other C files just include.
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef TMB_H_
#define TMB_H_
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#if defined(_WIN32) && defined(_MSC_VER) &&                                    \
        !(defined(__clang__) || defined(__GNUC__))
    #if defined(TMB_BUILD_STATIC) || defined(TMB_USING_STATIC)
        #define TMB_API
    #elif defined(TMB_BUILD_SHARED)
        #define TMB_API __declspec(dllexport)
    #else
        #define TMB_API __declspec(dllimport)
    #endif
#else
    #if defined(TMB_BUILD_SHARED) && defined(__GNUC__) && __GNUC__ >= 4
        #define TMB_API __attribute__((visibility("default")))
    #else
        #define TMB_API
    #endif
#endif

#if !defined(_MSC_VER) || defined(__clang__) || defined(__GNUC__)
    // https://clang.llvm.org/docs/AttributeReference.html#format
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)                                  \
        __attribute__((format(printf, STR_IDX, ARG_BEGIN)))
    #define TMB_BASEFILENAME __FILE_NAME__
#else // not supported on MSVC
    #define TMB_FMT_CHECK(STR_IDX, ARG_BEGIN)
    #define TMB_BASEFILENAME __FILE__
#endif

extern const char* const TMB_PATCH_V;
extern const char* const TMB_MINOR_V;
extern const char* const TMB_MAJOR_V;
extern const char* const TMB_SO_V;
extern const char* const TMB_GIT_REV;

#define TMB_MAX_LOGGER_NAME_LEN 64

#define TMB_LEVEL_NONE    (-1)
#define TMB_LEVEL_FATAL   0
#define TMB_LEVEL_ERROR   1
#define TMB_LEVEL_WARNING 2
#define TMB_LEVEL_INFO    3
#define TMB_LEVEL_DEBUG   4
#define TMB_LEVEL_TRACE   5
#define TMB_LEVEL_ALL     5

#ifndef TMB_MIN_LOG_LEVEL
    #define TMB_MIN_LOG_LEVEL TMB_LEVEL_TRACE
#endif

struct tmb_formatter;
struct tmb_log_ctx;
struct tmb_logger;
struct tmb_chip;
struct tmb_config;

typedef struct tmb_formatted_msg {
    int length;
    union {
        char* items;
        char* message;
    };
} tmb_formatted_msg_t;

typedef void tmb_free_fn_t(void*);
typedef void tmb_sink_fn_t(const char*, int, void*);
typedef tmb_formatted_msg_t tmb_format_fn_t(
        const struct tmb_formatter* formatter,
        const struct tmb_log_ctx* const ctx,
        const struct tmb_logger* lgr);

typedef enum {
    TMB_LOG_LEVEL_FATAL   = TMB_LEVEL_FATAL,
    TMB_LOG_LEVEL_ERROR   = TMB_LEVEL_ERROR,
    TMB_LOG_LEVEL_WARNING = TMB_LEVEL_WARNING,
    TMB_LOG_LEVEL_INFO    = TMB_LEVEL_INFO,
    TMB_LOG_LEVEL_DEBUG   = TMB_LEVEL_DEBUG,
    TMB_LOG_LEVEL_TRACE   = TMB_LEVEL_TRACE,

    TMB_LOG_LEVEL_COUNT
} tmb_log_level;

typedef struct tmb_lib_cfg {
    tmb_log_level log_level;
    bool enable_colors;
    int default_sink_fd; // either 1 (stdout) or 2 (stderr)
} tmb_lib_cfg_t;

/* Logger object */
typedef struct tmb_logger_cfg {
    tmb_log_level log_level;
    bool enable_colors;
} tmb_logger_cfg_t;
typedef struct tmb_sinks {
    int length;
    int capacity;
    struct tmb_sink** items;
} tmb_sinks_t;
typedef struct tmb_formatters {
    int length;
    int capacity;
    struct tmb_formatter* items;
} tmb_formatters_t;
typedef struct tmb_tag {
    const char* items;
    int length;
} tmb_tag_t;
typedef struct tmb_tags {
    tmb_tag_t* items;
    int capacity;
    int length;
} tmb_tags_t;
typedef struct tmb_logger {
    char name[TMB_MAX_LOGGER_NAME_LEN];
    tmb_logger_cfg_t cfg;
    tmb_sinks_t sinks;
    tmb_formatters_t formatters;
    tmb_tags_t tags;
    struct {
        int length;
        int capacity;
        int* items;
    } sink_formatter_map;
    int64_t last_message_stopwatch_sec;
    int64_t last_message_stopwatch_nsec;
    struct {
        bool stopwatch;
        bool timestamp;
    } has;
} tmb_logger_t;
/*logger object end*/

typedef struct tmb_log_ctx {
    const tmb_log_level log_level;
    const int line_no;
    const char* const filename;
    const int filename_len;
    const char* const filename_base;
    const int filename_base_len;
    const char* const funcname;
    const int funcname_len;
    char* message;
    int message_len;
    long long ts_sec;
    long long ts_nsec;
    long long stopwatch_sec;
    long long stopwatch_nsec;
} tmb_log_ctx_t;

typedef struct tmb_sink {
    tmb_sink_fn_t* sink_fn;
    tmb_free_fn_t* data_free_fn;
    void* sink_data;
    tmb_log_level min_log_level;
    atomic_int ref_count;
} tmb_sink_t;

struct tmb_sink_opts {
    tmb_log_level min_log_level;
};

typedef struct tmb_formatter {
    int length;
    int capacity;
    struct tmb_chip* items;
    void* data;
    tmb_format_fn_t* format_fn;
    tmb_free_fn_t* formated_free_fn;
    tmb_free_fn_t* data_free_fn;
    struct {
        bool stopwatch;
        bool timestamp;
    } has;
    struct {
        bool colors;
    } enable;
} tmb_formatter_t;

// used for macro trick
typedef struct tmb_json_formatter_opts {
    bool has_timestamp;
    bool has_stopwatch;
} tmb_json_formatter_opts_t;

/*
********************************************************************************
General library functions
********************************************************************************
 */
TMB_API void tmb_set_global_options(tmb_lib_cfg_t);
TMB_API void tmb_print_version(void);
TMB_API const char* tmb_get_version(void);
TMB_API struct tmb_logger* tmb_get_default_logger();

/* Logging functions */
TMB_API void tmb_log(tmb_log_ctx_t ctx,
                     tmb_logger_t* logger,
                     const char* message,
                     ...) TMB_FMT_CHECK(3, 4);
TMB_API void tmb_log_default(tmb_log_ctx_t ctx, const char* message, ...)
        TMB_FMT_CHECK(2, 3);

/* Config */
TMB_API struct tmb_config* tmb_config_load(const char* filename);
TMB_API struct tmb_config* tmb_config_from_string(const char* config);
TMB_API const char* tmb_config_get_format(struct tmb_config* config,
                                          const char* format_name);

/* Logger registry */
TMB_API void tmb_register_logger(const char* name, struct tmb_logger* logger);
TMB_API struct tmb_logger* tmb_get_logger(const char* name);
TMB_API struct tmb_logger* tmb_get_logger_or_default(const char* name);

/*
********************************************************************************
Logger
********************************************************************************
*/
TMB_API tmb_logger_t* tmb_logger_create(const char* logger_name,
                                        tmb_logger_cfg_t cfg);
TMB_API void tmb_logger_destroy(tmb_logger_t* logger);

TMB_API bool tmb_logger_set_default_format(tmb_logger_t* logger,
                                           const char* fmt);
TMB_API int tmb_logger_add_formatter(tmb_logger_t* lgr,
                                     tmb_formatter_t formatter);
TMB_API int tmb_logger_add_format(tmb_logger_t* lgr, const char* fmt);
TMB_API int tmb_logger_add_sink(tmb_logger_t* lgr, tmb_sink_t*);
TMB_API int tmb_logger_assign_format(tmb_logger_t* lgr,
                                     int sink_idx,
                                     int fmt_idx);
TMB_API void tmb_logger_add_tag(tmb_logger_t* lgr, const char* tag);
TMB_API void tmb_logger_remove_tag(tmb_logger_t* lgr, const char* tag);

/*
********************************************************************************
Formatter API
********************************************************************************
*/
TMB_API tmb_formatter_t tmb_formatter_make(const char* fmt);
TMB_API void tmb_formatter_deinit(tmb_formatter_t* formater);
/* Pre made formats */
TMB_API tmb_formatter_t tmb_formatter_graylog_make(void);
TMB_API tmb_formatter_t tmb_formatter_json_make(tmb_json_formatter_opts_t opts);

/*
********************************************************************************
Sinks
********************************************************************************
 */
TMB_API tmb_sink_t* tmb_sink_fd_create(struct tmb_sink_opts opts, FILE* fd);
TMB_API tmb_sink_t* tmb_sink_file_create(struct tmb_sink_opts opts,
                                         const char* filename);
TMB_API tmb_sink_t* tmb_sink_rotating_file_create(struct tmb_sink_opts opts,
                                                  const char* filename,
                                                  int max_files,
                                                  int max_file_size_B);
TMB_API tmb_sink_t* tmb_sink_null_create();
TMB_API tmb_sink_t* tmb_sink_graylog_create(struct tmb_sink_opts opts,
                                            const char* graylog_host,
                                            int port);
TMB_API void tmb_sink_destroy(tmb_sink_t* sink);

/*
********************************************************************************
Library macors
********************************************************************************
 */
#define TMB_DEFAULT_LIB_CFG                                                    \
    .enable_colors = true, .log_level = TMB_LOG_LEVEL_INFO, .default_sink_fd = 2

#define TMB_CFG(...)                                                           \
    tmb_set_global_options(                                                    \
            (struct tmb_lib_cfg) { TMB_DEFAULT_LIB_CFG, __VA_ARGS__ })

/*
********************************************************************************
Logger Creation macros
********************************************************************************
*/
#define TMB_DEFAULT_LOGGER_CFG                                                 \
    .enable_colors = true, .log_level = TMB_LOG_LEVEL_INFO
#define TMB_LOGGER(_m_name, ...)                                               \
    tmb_logger_create(                                                         \
            _m_name,                                                           \
            (struct tmb_logger_cfg) { TMB_DEFAULT_LOGGER_CFG, __VA_ARGS__ })

/*
********************************************************************************
Premade formatter macros
********************************************************************************
*/
#define TMB_FORMAT_GRAYLOG() tmb_formatter_graylog_make()

#define TMB_FORMAT_JSON(...)                                                   \
    tmb_formatter_json_make((tmb_json_formatter_opts_t) { __VA_ARGS__ })

/*
********************************************************************************
Sink Creation macros
********************************************************************************
*/
#define TMB_SINK_DEFAULT_OPTS .min_log_level = TMB_LOG_LEVEL_DEBUG

#define TMB_SINK_NULL() tmb_sink_null_create()

#define TMB_SINK_STDOUT(...)                                                   \
    tmb_sink_fd_create(                                                        \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            stdout)
#define TMB_SINK_STDERR(...)                                                   \
    tmb_sink_fd_create(                                                        \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            stderr)
#define TMB_SINK_FD(fd, ...)                                                   \
    tmb_sink_fd_create(                                                        \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ }, fd)

#define TMB_SINK_FILE(filename, ...)                                           \
    tmb_sink_file_create(                                                      \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            filename)

#define TMB_SINK_ROTATING_FILE(filename, max_files, max_file_size_B, ...)      \
    tmb_sink_rotating_file_create(                                             \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            filename,                                                          \
            max_files,                                                         \
            max_file_size_B)

#define TMB_SINK_GRAYLOG(graylog_host, port, ...)                              \
    tmb_sink_graylog_create(                                                   \
            (struct tmb_sink_opts) { TMB_SINK_DEFAULT_OPTS, __VA_ARGS__ },     \
            graylog_host,                                                      \
            port)

// clang-format off
#if TMB_LEVEL_FATAL <= TMB_MIN_LOG_LEVEL
    #define TMB_LOG_FATAL(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_FATAL, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_LOG_FATAL(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_ERROR <= TMB_MIN_LOG_LEVEL
    #define TMB_LOG_ERROR(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_ERROR, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_LOG_ERROR(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_WARNING <= TMB_MIN_LOG_LEVEL
    #define TMB_LOG_WARNING(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_WARNING, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
    #define TMB_LOG_WARN(lgr_or_fmt, ...) TMB_LOG_WARNING(lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_LOG_WARN(lgr_or_fmt, ...)
    #define TMB_LOG_WARNING(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_INFO <= TMB_MIN_LOG_LEVEL
    #define TMB_LOG_INFO(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_INFO, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_LOG_INFO(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_DEBUG <= TMB_MIN_LOG_LEVEL
    #define TMB_LOG_DEBUG(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_DEBUG, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_LOG_DEBUG(lgr_or_fmt, ...)
#endif

#if TMB_LEVEL_TRACE <= TMB_MIN_LOG_LEVEL
    #define TMB_LOG_TRACE(lgr_or_fmt, ...) TMB_LOG(TMB_LEVEL_TRACE, lgr_or_fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TMB_LOG_TRACE(lgr_or_fmt, ...)
#endif


#ifndef TMB_DONT_STRIP_PREFIX
#define LOG_FATAL TMB_LOG_FATAL
#define LOG_ERROR TMB_LOG_ERROR
#define LOG_WARNING TMB_LOG_WARNING
#define LOG_INFO TMB_LOG_INFO
#define LOG_DEBUG TMB_LOG_DEBUG
#define LOG_TRACE TMB_LOG_TRACE
#endif
// clang-format on

#define TMB_CALL(func, ...)   func(__VA_ARGS__)
#define TMB_CONST_STR_SIZE(X) (sizeof(X) - 1)

#define TMB_DISPATCH(ctx, logger_or_format, ...)                               \
    TMB_CALL(_Generic((logger_or_format),                                      \
                     tmb_logger_t*: tmb_log,                                   \
                     char*: tmb_log_default),                                  \
             ctx,                                                              \
             logger_or_format __VA_OPT__(, ) __VA_ARGS__)

#define TMB_LOG(_log_level, logger_or_format, ...)                             \
    do {                                                                       \
        tmb_log_ctx_t _m__ctx = {                                              \
            .log_level         = _log_level,                                   \
            .line_no           = __LINE__,                                     \
            .filename          = __FILE__,                                     \
            .filename_len      = TMB_CONST_STR_SIZE(__FILE__),                 \
            .filename_base     = TMB_BASEFILENAME,                             \
            .filename_base_len = TMB_CONST_STR_SIZE(TMB_BASEFILENAME),         \
            .funcname          = __func__,                                     \
            .funcname_len      = TMB_CONST_STR_SIZE(__func__)                  \
        };                                                                     \
        TMB_DISPATCH(_m__ctx, logger_or_format __VA_OPT__(, ) __VA_ARGS__);    \
    } while (0)

#ifdef TMB_LOGGING_IMPLEMENTATION

    #include <stdlib.h>

static tmb_lib_cfg_t g_opts = { TMB_DEFAULT_LIB_CFG };

void tmb_set_global_options(tmb_lib_cfg_t opts) { g_opts = opts; }

void tmb_print_version(void) { printf("%s", tmb_get_version()); }
const char* tmb_get_version(void) {
    return "timber @ header only\n"
           "Host program compiled at " __DATE__ " " __TIME__ "\n";
}
struct tmb_logger* tmb_get_default_logger() { return NULL; }

static void tmb_log_v(tmb_log_ctx_t ctx, const char* message, va_list args) {
    if (ctx.log_level > g_opts.log_level) { return; }
    va_list args1;
    va_copy(args1, args);
    int n = vsnprintf(NULL, 0, message, args1);
    va_end(args1);
    char* msg = malloc((size_t)n + 1);
    vsnprintf(msg, (size_t)n + 1, message, args);

    char* log_lvl       = NULL;
    char* log_lvl_color = NULL;
    char* color_reset   = "\x1b[0m";
    switch (ctx.log_level) {
    case TMB_LOG_LEVEL_FATAL:
        log_lvl       = "FATAL";
        log_lvl_color = "\x1b[1;31m";
        break;
    case TMB_LOG_LEVEL_ERROR:
        log_lvl       = "ERROR";
        log_lvl_color = "\x1b[31m";
        break;
    case TMB_LOG_LEVEL_WARNING:
        log_lvl       = "WARN ";
        log_lvl_color = "\x1b[33m";
        break;
    case TMB_LOG_LEVEL_INFO:
        log_lvl       = "INFO ";
        log_lvl_color = "\x1b[32m";
        break;
    case TMB_LOG_LEVEL_DEBUG:
        log_lvl       = "DEBUG";
        log_lvl_color = "\x1b[34m";
        break;
    case TMB_LOG_LEVEL_TRACE:
        log_lvl       = "TRACE";
        log_lvl_color = "\x1b[1;30m";
        break;
    case TMB_LOG_LEVEL_COUNT:
    default:
        log_lvl       = "Unknown log level :(";
        log_lvl_color = "\x1b[1;31m";
        break;
    }
    FILE* fd = NULL;
    switch (g_opts.default_sink_fd) {
    case 1:
        fd = stdout;
        break;
    case 2:
    default:
        fd = stderr;
    }

    if (!g_opts.enable_colors) {
        log_lvl_color = "";
        color_reset   = "";
    }

    (void)fprintf(fd,
                  "%s[%s]%s - %s:%d - %s%s%s\n",
                  log_lvl_color,
                  log_lvl,
                  color_reset,
                  ctx.filename_base,
                  ctx.line_no,
                  log_lvl_color,
                  msg,
                  color_reset);

    free(msg);
}

void tmb_log(tmb_log_ctx_t ctx, tmb_logger_t*, const char* message, ...) {
    va_list args;
    va_start(args, message);
    tmb_log_v(ctx, message, args);
    va_end(args);
}
void tmb_log_default(tmb_log_ctx_t ctx, const char* message, ...) {
    va_list args;
    va_start(args, message);
    tmb_log_v(ctx, message, args);
    va_end(args);
}

struct tmb_config* tmb_config_load(const char*) { return NULL; }
struct tmb_config* tmb_config_from_string(const char*) { return NULL; }
const char* tmb_config_get_format(struct tmb_config*, const char*) {
    return "";
}

void tmb_register_logger(const char*, struct tmb_logger*) {}
struct tmb_logger* tmb_get_logger(const char*) { return NULL; }
struct tmb_logger* tmb_get_logger_or_default(const char*) { return NULL; }

tmb_logger_t* tmb_logger_create(const char*, tmb_logger_cfg_t) { return NULL; }
void tmb_logger_destroy(tmb_logger_t*) {}

bool tmb_logger_set_default_format(tmb_logger_t*, const char*) { return true; }
int tmb_logger_add_formatter(tmb_logger_t*, tmb_formatter_t) { return 0; }
int tmb_logger_add_format(tmb_logger_t*, const char*) { return 0; }
int tmb_logger_add_sink(tmb_logger_t*, tmb_sink_t*) { return 0; }
int tmb_logger_assign_format(tmb_logger_t*, int, int) { return 0; }
void tmb_logger_add_tag(tmb_logger_t*, const char*) {}
void tmb_logger_remove_tag(tmb_logger_t*, const char*) {}

tmb_formatter_t tmb_formatter_make(const char*) {
    return (tmb_formatter_t) { 0 };
}
void tmb_formatter_deinit(tmb_formatter_t*) {}
tmb_formatter_t tmb_formatter_graylog_make(void) {
    return (tmb_formatter_t) { 0 };
}
tmb_formatter_t tmb_formatter_json_make(tmb_json_formatter_opts_t) {
    return (tmb_formatter_t) { 0 };
}

tmb_sink_t* tmb_sink_fd_create(struct tmb_sink_opts, FILE*) { return NULL; }
tmb_sink_t* tmb_sink_file_create(struct tmb_sink_opts, const char*) {
    return NULL;
}
tmb_sink_t* tmb_sink_rotating_file_create(struct tmb_sink_opts,
                                          const char*,
                                          int,
                                          int) {
    return NULL;
}
tmb_sink_t* tmb_sink_null_create() { return NULL; }
tmb_sink_t* tmb_sink_graylog_create(struct tmb_sink_opts, const char*, int) {
    return NULL;
}
void tmb_sink_destroy(tmb_sink_t*) { return; }

#endif // TMB_LOGGING_IMPLEMENTATION

#endif // TMB_H_
