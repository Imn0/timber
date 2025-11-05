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
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

struct tmb_formatter;
struct tmb_log_ctx;
struct tmb_logger;

typedef struct tmb_formatted_msg {
    int length;
    union {
        char* items;
        char* message;
    };
} tmb_formatted_msg_t;

typedef void tmb_free_fn_t(void*);
typedef void tmb_sink_fn_t(const char*, int, void*);
typedef tmb_formatted_msg_t tmb_format_fn_t(struct tmb_formatter* formatter,
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

#define TMB_DEFAULT_LIB_CFG .enable_colors = true, .log_level = LOG_LEVEL_DEBUG

#define TMB_DEFAULT_LOGGER_CFG                                                 \
    .enable_colors = true, .log_level = TMB_LOG_LEVEL_INFO

typedef struct tmb_sink {
    tmb_sink_fn_t* sink_fn;
    tmb_free_fn_t* free_fn;
    void* sink_data;
    atomic_int ref_count;
} tmb_sink_t;

struct tmb_chip;

typedef struct tmb_sinks {
    int length;
    int capacity;
    struct tmb_sink** items;
} tmb_sinks_t;

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
        bool time_stamp;
    } has;
} tmb_formatter_t;

typedef struct tmb_formatters {
    int length;
    int capacity;
    struct tmb_formatter* items;
} tmb_formatters_t;

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

typedef struct tmb_cfg {
    tmb_log_level log_level;
    bool enable_colors;
} tmb_cfg_t;

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
    tmb_cfg_t cfg;
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
        bool time_stamp;
    } has;
} tmb_logger_t;

/* Library functions */
TMB_API void tmb_set_options(tmb_cfg_t);
TMB_API void tmb_print_version(void);
TMB_API const char* tmb_get_version(void);
TMB_API tmb_logger_t* tmb_get_default_logger();

/* Logger functions */
TMB_API tmb_logger_t* tmb_logger_create(const char* logger_name, tmb_cfg_t cfg);
TMB_API void tmb_logger_destroy(tmb_logger_t* logger);

TMB_API bool tmb_logger_set_default_format(tmb_logger_t* logger,
                                           const char* fmt);
TMB_API int tmb_logger_add_formatter(tmb_logger_t* lgr,
                                     tmb_formatter_t formatter);
TMB_API int tmb_logger_add_format(tmb_logger_t* lgr, const char* fmt);
TMB_API int tmb_logger_add_sink(tmb_logger_t* lgr, tmb_sink_t*);
TMB_API void tmb_logger_add_tag(tmb_logger_t* lgr, const char* tag);
TMB_API void tmb_logger_remove_tag(tmb_logger_t* lgr, const char* tag);
TMB_API int tmb_logger_assign_format(tmb_logger_t* lgr,
                                     int sink_idx,
                                     int fmt_idx);

/* Logger registry */
TMB_API void tmb_register_logger(const char* name, tmb_logger_t* logger);
TMB_API tmb_logger_t* tmb_get_logger(const char* name);
TMB_API tmb_logger_t* tmb_get_logger_or_default(const char* name);

/* Format functions */
TMB_API tmb_formatter_t tmb_formatter_make(const char* fmt);
TMB_API tmb_formatter_t tmb_formatter_graylog_make(void);
TMB_API void tmb_formatter_deinit(tmb_formatter_t* formater);

/* Logging functions */
TMB_API void tmb_log(tmb_log_ctx_t ctx,
                     tmb_logger_t* logger,
                     const char* message,
                     ...) TMB_FMT_CHECK(3, 4);
TMB_API void tmb_log_default(tmb_log_ctx_t ctx, const char* message, ...)
        TMB_FMT_CHECK(2, 3);

#ifndef TMB_MIN_LOG_LEVEL
    #define TMB_MIN_LOG_LEVEL TMB_LEVEL_DEBUG
#endif

#define TMB_CALL(func, ...)   func(__VA_ARGS__)
#define TMB_CONST_STR_SIZE(X) (sizeof(X) - 1)

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

#define TMB_CFG(...)                                                           \
    tmb_set_options((struct tmb_cfg) { .log_level     = TMB_LOG_LEVEL_INFO,    \
                                       .enable_colors = true,                  \
                                       __VA_ARGS__ })

#define TMB_LOGGER(_m_name, ...)                                               \
    tmb_logger_create(                                                         \
            _m_name, (struct tmb_cfg) { TMB_DEFAULT_LOGGER_CFG, __VA_ARGS__ })

#ifdef TMB_LOGGING_IMPLEMENTATION
// TODO
#endif // TMB_LOGGING_IMPLEMENTATION

#endif // TMB_H_
