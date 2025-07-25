#ifndef TMB_FORMAT_H
#define TMB_FORMAT_H

#include <tmb_lib.h>

void tmb_formatter_add_chip(Formatter* fmt, FormatToken chip);
void tmb_formatter_add_chips(Formatter* fmt, size_t count, ...);

FormatToken tmb_fmt_chip_const_val_make(const char* value);
FormatToken tmb_fmt_chip_message_make();
FormatToken tmb_fmt_chip_log_level_make(bool use_color);

#endif // TMB_FORMAT_H
