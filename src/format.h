#ifndef TMB_FORMAT_H
#define TMB_FORMAT_H

#include <tmb_lib.h>

typedef enum {
    FMT_FN,
    FMT_MSG
} FormatTokenType;

typedef struct {
    FormatTokenType type;
    void* token_data;
    fmt_fn_t* fmt_function;
    free_fn_t* free_fn;
} FormatToken;

typedef struct {
    int token_count;
    FormatToken tokens[];
} Formatter;

void const_format_token_init(FormatToken* fmt, cstr value);

void filename_format_token_init(FormatToken* fmt);

void log_level_format_init(FormatToken* fmt, bool use_color);

#endif // TMB_FORMAT_H
