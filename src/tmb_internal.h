#ifndef TMB_INTERNAL_H_
#define TMB_INTERNAL_H_

#include <tmb.h>
#include <tmb_lib.h>

#include <time.h>

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
    const char* const funcname;
    const int funcname_len;
    const time_t log_timestamp;
    const char* const message;
    const int message_len;
    const tmb_time_stamp_t ts;
} tmb_log_ext_ctx_t;

/* Library functions */
TMB_API TMB_INIT void tmb_init(void);
TMB_API TMB_DEINIT void tmb_deinit(void);

extern const char* const tmb_log_level_str[TMB_LOG_LEVEL_COUNT];
extern const char* const tmb_log_level_color[TMB_LOG_LEVEL_COUNT];
extern int const tmb_log_level_str_len[TMB_LOG_LEVEL_COUNT];
extern const char tmb_log_level_char[TMB_LOG_LEVEL_COUNT];

#endif
