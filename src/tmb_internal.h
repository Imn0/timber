#ifndef TMB_INTERNAL_H_
#define TMB_INTERNAL_H_

#include <tmb/tmb.h>
#include <tmb_lib.h>

#include <time.h>

#ifndef TMB_LEVEL_FATAL_STR
    #define TMB_LEVEL_FATAL_STR "FATAL"
#endif
#ifndef TMB_LEVEL_ERROR_STR
    #define TMB_LEVEL_ERROR_STR "ERROR"
#endif
#ifndef TMB_LEVEL_WARNING_STR
    #define TMB_LEVEL_WARNING_STR "WARNING"
#endif
#ifndef TMB_LEVEL_INFO_STR
    #define TMB_LEVEL_INFO_STR "INFO"
#endif
#ifndef TMB_LEVEL_DEBUG_STR
    #define TMB_LEVEL_DEBUG_STR "DEBUG"
#endif
#ifndef TMB_LEVEL_TRACE_STR
    #define TMB_LEVEL_TRACE_STR "TRACE"
#endif

#ifndef TMB_LEVEL_FATAL_COLOR
    #define TMB_LEVEL_FATAL_COLOR MAKE_ANSI(ANSI_RED, ANSI_BOLD)
#endif
#ifndef TMB_LEVEL_ERROR_COLOR
    #define TMB_LEVEL_ERROR_COLOR MAKE_ANSI(ANSI_RED)
#endif
#ifndef TMB_LEVEL_WARNING_COLOR
    #define TMB_LEVEL_WARNING_COLOR MAKE_ANSI(ANSI_YELLOW)
#endif
#ifndef TMB_LEVEL_INFO_COLOR
    #define TMB_LEVEL_INFO_COLOR MAKE_ANSI(ANSI_GREEN)
#endif
#ifndef TMB_LEVEL_DEBUG_COLOR
    #define TMB_LEVEL_DEBUG_COLOR MAKE_ANSI(ANSI_BLUE)
#endif
#ifndef TMB_LEVEL_TRACE_COLOR
    #define TMB_LEVEL_TRACE_COLOR MAKE_ANSI(ANSI_BLACK, ANSI_ITALIC)
#endif

#ifndef TMB_DEFAULT_FORMAT
    #define TMB_DEFAULT_FORMAT "%@:%# [%7^l] %$\n"
#endif

#define JUSTING_LEFT_OPT_CHAR   '<'
#define JUSTING_RIGHT_OPT_CHAR  '>'
#define JUSTING_CENTER_OPT_CHAR '^'

#define TRUNCATING_LEFT_OPT_CHAR  '['
#define TRUNCATING_RIGHT_OPT_CHAR ']'

typedef struct tmb_log_ext_ctx {
    const tmb_log_level log_level;
    const int line_no;
    const char* const filename;
    const int filename_len;
    const char* const filename_base;
    const int filename_base_len;
    const char* const funcname;
    const int funcname_len;
    const time_t log_timestamp;
    const char* const message;
    const int message_len;
    const tmb_time_stamp_t ts;
} tmb_log_ext_ctx_t;

typedef struct tmb_logger_registry {
    _hm_header_(const char*, tmb_logger_t*);
} tmb_logger_registry_t;

extern tmb_logger_registry_t tmb_logger_registry;
extern const char* const tmb_log_level_str[TMB_LOG_LEVEL_COUNT];
extern const char* const tmb_log_level_color[TMB_LOG_LEVEL_COUNT];
extern int const tmb_log_level_str_len[TMB_LOG_LEVEL_COUNT];
extern const char tmb_log_level_char[TMB_LOG_LEVEL_COUNT];

bool tmb_format_init(tmb_formater_t* format, const char* fmt);

#endif
