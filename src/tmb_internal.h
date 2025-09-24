#ifndef TMB_INTERNAL_H_
#define TMB_INTERNAL_H_

#include <tmb/tmb.h>
#include <tmb_lib.h>

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
    #define TMB_DEFAULT_FORMAT "[{^7:l}] - {$BLACK:$DIM:}{>]20:@}:{<4:#}{$RESET:} {$}\n"
#endif

#define JUSTING_LEFT_OPT_CHAR   '<'
#define JUSTING_RIGHT_OPT_CHAR  '>'
#define JUSTING_CENTER_OPT_CHAR '^'

#define TRUNCATING_LEFT_OPT_CHAR  '['
#define TRUNCATING_RIGHT_OPT_CHAR ']'

typedef struct tmb_logger_registry {
    _hm_header_(const char*, tmb_logger_t*);
} tmb_logger_registry_t;

extern tmb_logger_registry_t tmb_logger_registry;
extern const char* const tmb_log_level_str[LOG_LEVEL_COUNT];
extern int const tmb_log_level_str_len[LOG_LEVEL_COUNT];
extern const char* const tmb_log_level_color[LOG_LEVEL_COUNT];
extern int const tmb_log_level_color_len[LOG_LEVEL_COUNT];
extern const char tmb_log_level_char[LOG_LEVEL_COUNT];

bool tmb_formatter_init(tmb_formatter_t* format, const char* fmt);
void tmb_formatter_print(const tmb_formatter_t* formatter);

#endif
