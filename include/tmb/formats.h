#ifndef TMB_FORMATS_H_
#define TMB_FORMATS_H_
#include "tmb.h"

TMB_API tmb_formatter_t tmb_formatter_graylog_make(void);
#define TMB_FORMAT_GRAYLOG() tmb_formatter_graylog_make()

typedef struct tmb_json_formatter_opts {
    bool has_timestamp;
    bool has_stopwatch;
} tmb_json_formatter_opts_t;

TMB_API tmb_formatter_t tmb_formatter_json_make(tmb_json_formatter_opts_t opts);
#define TMB_FORMAT_JSON(...)                                                   \
    tmb_formatter_json_make((tmb_json_formatter_opts_t) { __VA_ARGS__ })

#endif // TMB_FORMATS_H_
