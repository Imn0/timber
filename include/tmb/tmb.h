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
#include <stdbool.h>
#include <stddef.h>

#if defined(_WIN32) && defined(_MSC_VER) &&                                    \
        !(defined(__clang__) || defined(__GNUC__))
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
extern const char* const GIT_REV;

#define TMB_LEVEL_NONE    (-1)
#define TMB_LEVEL_FATAL   0
#define TMB_LEVEL_ERROR   1
#define TMB_LEVEL_WARNING 2
#define TMB_LEVEL_INFO    3
#define TMB_LEVEL_DEBUG   4
#define TMB_LEVEL_TRACE   5
#define TMB_LEVEL_ALL     5

typedef void tmb_free_fn_t(void*);
typedef void tmb_sink_fn_t(const char*, int, void*);

typedef enum {
    TMB_LOG_LEVEL_FATAL   = TMB_LEVEL_FATAL,
    TMB_LOG_LEVEL_ERROR   = TMB_LEVEL_ERROR,
    TMB_LOG_LEVEL_WARNING = TMB_LEVEL_WARNING,
    TMB_LOG_LEVEL_INFO    = TMB_LEVEL_INFO,
    TMB_LOG_LEVEL_DEBUG   = TMB_LEVEL_DEBUG,
    TMB_LOG_LEVEL_TRACE   = TMB_LEVEL_TRACE,

    TMB_LOG_LEVEL_COUNT
} tmb_log_level;

typedef struct tmb_sink {
    tmb_sink_fn_t* sink_fn;
    tmb_free_fn_t* free_fn;
    void* sink_data;
} tmb_sink_t;

struct tmb_chip;

typedef struct tmb_sinks {
    int length;
    int capacity;
    struct tmb_sink* items;
} tmb_sinks_t;

typedef struct tmb_formater {
    int length;
    int capacity;
    struct tmb_chip* items;
} tmb_formater_t;

typedef struct {
    const tmb_log_level log_level;
    const int line_no;
    const char* const filename;
    const int filename_len;
    const char* const filename_base;
    const int filename_base_len;
    const char* const funcname;
    const int funcname_len;
} tmb_log_ctx_t;

typedef struct tmb_logger {
    tmb_sinks_t sinks;
    tmb_log_level log_level;
    struct {
        int length;
        int capacity;
        tmb_formater_t* items;
    } formatters;
    struct {
        int length;
        int capacity;
        int* items;
    } sink_formatter_map;
} tmb_logger_t;

typedef struct tmb_tee_logger {
    int length;
    int capacity;
    tmb_logger_t** items;
} tmb_tee_logger_t;

/* Library functions */
TMB_API void tmb_print_version(void);
TMB_API const char* tmb_get_version(void);

/* Logger functions */
TMB_API tmb_logger_t* tmb_get_default_logger();
TMB_API bool tmb_logger_set_default_format(tmb_logger_t* logger,
                                           const char* fmt);
TMB_API int tmb_logger_add_format(tmb_logger_t* lgr, const char* fmt);
TMB_API int tmb_logger_add_sink(tmb_logger_t* lgr, tmb_sink_t);
TMB_API int tmb_logger_assign_format(tmb_logger_t* lgr,
                                     int sink_idx,
                                     int fmt_idx);
TMB_API int tmb_logger_set_format(tmb_logger_t* lgr,
                                  int sink_idx,
                                  const char* fmt);

TMB_API void tmb_tee_logger_add_logger(tmb_tee_logger_t* tee_logger,
                                       tmb_logger_t* lgr);

TMB_API void tmb_register_logger(const char* name, tmb_logger_t* logger);
TMB_API tmb_logger_t* tmb_get_logger(const char* name);
TMB_API tmb_logger_t* tmb_get_logger_or_default(const char* name);

/* Logging functions */
TMB_API void tmb_log(tmb_log_ctx_t ctx,
                     tmb_logger_t* logger,
                     const char* message,
                     ...) TMB_FMT_CHECK(3, 4);
TMB_API void tmb_log_default(tmb_log_ctx_t ctx, const char* message, ...)
        TMB_FMT_CHECK(2, 3);
TMB_API void tmb_tee_log(tmb_log_ctx_t ctx,
                         const tmb_tee_logger_t* tee_logger,
                         const char* message,
                         ...) TMB_FMT_CHECK(3, 4);

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
                     tmb_logger_t*: tmb_log,                                   \
                     tmb_tee_logger_t*: tmb_tee_log,                           \
                     char*: tmb_log_default),                                  \
             ctx,                                                              \
             logger_or_format __VA_OPT__(, ) __VA_ARGS__)

#define TMB_LOG(log_level, logger_or_format, ...)                              \
    do {                                                                       \
        tmb_log_ctx_t _m__ctx = {                                              \
            log_level,        __LINE__,                                        \
            __FILE__,         TMB_CONST_STR_SIZE(__FILE__),                    \
            TMB_BASEFILENAME, TMB_CONST_STR_SIZE(TMB_BASEFILENAME),            \
            __func__,         TMB_CONST_STR_SIZE(__func__)                     \
        };                                                                     \
        TMB_DISPATCH(_m__ctx, logger_or_format __VA_OPT__(, ) __VA_ARGS__);    \
    } while (0)

#ifdef TMB_LOGGING_IMPLEMENTATION
// TODO
#endif // TMB_LOGGING_IMPLEMENTATION

#endif // TMB_H_
