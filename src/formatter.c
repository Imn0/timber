#include <ctype.h>
#include <formatter.h>

static inline void tmb_chip_format(tmb_chip_t* chip,
                                   tmb_string_builder_t* target,
                                   const tmb_log_ctx_t* const ctx,
                                   bool use_color) {
    tmb_string_builder_t buff       = { 0 };
    tmb_string_builder_t color_buff = { 0 };
    bool color_used                 = false;
    switch (chip->type) {
    case CHIP_TYPE_UNKNOWN:
        printf("ERROR CHIP\n");
        break;
    case CHIP_TYPE_MESSAGE:
        sb_appendn(&buff, ctx->message_len, ctx->message);
        break;
    case CHIP_TYPE_CONST_VAL:
        sb_appendn(&buff, chip->const_val.length, chip->const_val.items);
        break;
    case CHIP_TYPE_LEVEL_L:
        if (use_color) {
            sb_appendn(&color_buff,
                       tmb_log_level_color_len[ctx->log_level],
                       tmb_log_level_color[ctx->log_level]);
            color_used = true;
        }
        sb_appendn(&buff,
                   tmb_log_level_str_len[ctx->log_level],
                   tmb_log_level_str[ctx->log_level]);
        break;
    case CHIP_TYPE_LEVEL_S:
        if (use_color) {
            sb_appendn(&color_buff,
                       tmb_log_level_color_len[ctx->log_level],
                       tmb_log_level_color[ctx->log_level]);
            color_used = true;
        }
        sb_append(&buff, tmb_log_level_char[ctx->log_level]);
        break;
    case CHIP_TYPE_BASEFILE:
        sb_appendn(&buff, ctx->filename_base_len, ctx->filename_base);
        break;
    case CHIP_TYPE_FILE:
        sb_appendn(&buff, ctx->filename_len, ctx->filename);
        break;
    case CHIP_TYPE_LINE:
        sb_appendf(&buff, "%d", ctx->line_no);
        break;
    case CHIP_TYPE_FUNC:
        sb_appendn(&buff, ctx->funcname_len, ctx->funcname);
        break;
    case CHIP_TYPE_COLOR:
        // todo
        break;
    default:
        break;
    }
    if (buff.length < chip->just_amount) {
        tmb_sb_just(&buff, chip->just_opt, chip->just_amount, ' ');
    } else if (buff.length > chip->just_amount) {
        tmb_sb_truncate(&buff, chip->truncate_opt, chip->just_amount);
    }
    if (color_used) {
        sb_appendn(&color_buff, buff.length, buff.items);
        sb_appendn(&color_buff,
                   (int)TMB_CONST_STR_SIZE(MAKE_ANSI(ANSI_RESET)),
                   MAKE_ANSI(ANSI_RESET));
        sb_appendn(target, color_buff.length, color_buff.items);
        sb_free(&buff);
        sb_free(&color_buff);
    } else {
        sb_appendn(target, buff.length, buff.items);
    }
    sb_free(&buff);
}

static tmb_formatted_msg_t tmb_format(tmb_formatter_t* formatter,
                                      const tmb_log_ctx_t* const ctx,
                                      tmb_format_opt_t opt) {
    tmb_string_builder_t message = { 0 };
    for (int i = 0; i < formatter->length; i++) {
        tmb_chip_t* current_chip = &formatter->items[i];
        tmb_chip_format(current_chip, &message, ctx, opt.use_color);
    }
    sb_to_cstr(&message);
    return (tmb_formatted_msg_t) { .items  = message.items,
                                   .length = message.length };
}

typedef struct tmb_chip_opts {
    _da_header_(tmb_string_view_t*);
} tmb_chip_opts_t;

static bool tmb_formatter_add_chip_from_opt(tmb_formatter_t* formatter,
                                            tmb_string_view_t* chip_type,
                                            tmb_chip_opts_t* opts) {
    tmb_chip_t chip = { 0 };

    UNUSED opts;
#define CASE(c, new_type)                                                      \
    case c:                                                                    \
        chip.type = new_type;                                                  \
        break
    bool only_opts = false;
    bool do_color  = false;

    if (chip_type->length > 1) { return false; }
    if (chip_type->length > 0) {
        switch (chip_type->items[0]) {
            CASE('$', CHIP_TYPE_MESSAGE);
            CASE('l', CHIP_TYPE_LEVEL_L);
            CASE('L', CHIP_TYPE_LEVEL_S);
            CASE('@', CHIP_TYPE_BASEFILE);
            CASE('s', CHIP_TYPE_FILE);
            CASE('#', CHIP_TYPE_LINE);
            CASE('f', CHIP_TYPE_FUNC);
        default:
            UNUSED fprintf(stderr, "unknown format %c\n", chip_type->items[0]);
            return false;
            break;
        }
    } else {
        only_opts = true;
        chip.type = CHIP_TYPE_COLOR;
    }

    enum tmb_sb_truncate_opt truncate_opt = TRUNCATE_OFF;
    enum tmb_sb_just_opt just_opt         = JUST_OFF;
    int just_amount                       = 0;

    for (int i = 0; i < opts->length; i++) {
        tmb_string_view_t* current_opt = &opts->items[i];
        if (current_opt->length <= 0) { continue; }
        if (current_opt->items[0] == '$') {
            tmb_chip_t color_chip = { .type = CHIP_TYPE_COLOR };
            da_append(formatter, color_chip);
            continue;
        }
        // we are left with just justing options []<>^ and a number
        int j = 0;
        while (j < current_opt->length) {
            char current_opt_char = current_opt->items[j];
            if (current_opt_char == JUSTING_LEFT_OPT_CHAR ||
                current_opt_char == JUSTING_CENTER_OPT_CHAR ||
                current_opt_char == JUSTING_RIGHT_OPT_CHAR) {
                switch (current_opt_char) {
                case JUSTING_LEFT_OPT_CHAR:
                    just_opt = JUST_LEFT;
                    break;
                case JUSTING_CENTER_OPT_CHAR:
                    just_opt = JUST_CENTER;
                    break;
                case JUSTING_RIGHT_OPT_CHAR:
                    just_opt = JUST_RIGHT;
                    break;
                default:
                    unreachable();
                    break;
                }
            }
            if (current_opt_char == TRUNCATING_LEFT_OPT_CHAR ||
                current_opt_char == TRUNCATING_RIGHT_OPT_CHAR) {
                switch (current_opt_char) {
                case TRUNCATING_LEFT_OPT_CHAR:
                    truncate_opt = TRUNCATE_LEFT;
                    break;
                case TRUNCATING_RIGHT_OPT_CHAR:
                    truncate_opt = TRUNCATE_RIGHT;
                    break;
                default:
                    unreachable();
                    break;
                }
            }
            if (isdigit(current_opt_char)) {
                just_amount *= 10;
                just_amount += (current_opt_char - '0');
            }

            j++;
        }
    }

    if (do_color && !only_opts) {
        tmb_chip_t color_chip = { .type = CHIP_TYPE_COLOR };
        da_append(formatter, color_chip);
    }

    chip.just_amount  = just_amount;
    chip.truncate_opt = truncate_opt;
    chip.just_opt     = just_opt;
    da_append(formatter, chip);
    return true;
}

static bool tmb_formatter_add_const_chip(tmb_formatter_t* formatter,
                                         const char* val,
                                         int val_len) {
    tmb_chip_t chip  = { 0 };
    chip.type        = CHIP_TYPE_CONST_VAL;
    chip.just_amount = JUST_OFF;

    tmb_string_builder_t sb = { 0 };
    sb_appendn(&sb, val_len, val);

    chip.const_val = sv_from_sb(&sb);
    da_append(formatter, chip);
    return true;
}

static void sv_init_or_extend(tmb_string_view_t* sv,
                              const tmb_string_view_t* const sv_of_what,
                              int idx) {
    if (sv->items == NULL) {
        sv->items  = sv_of_what->items + idx;
        sv->length = 1;
    } else {
        sv->length++;
    }
}

static void sv_reset(tmb_string_view_t* sv) {
    sv->items  = NULL;
    sv->length = 0;
}

bool tmb_formatter_init(tmb_formatter_t* formatter, const char* fmt) {
    const tmb_string_view_t sv = { .items = fmt, .length = (int)strlen(fmt) };
    tmb_string_view_t buff     = { 0 };

    tmb_chip_opts_t opts = { 0 };

    for (int i = 0; i < sv.length; i++) {
        // handle lone '}' for consistency with {{ -> {
        if (sv.items[i] == '}' && sv.items[i + 1] != '}') {
            printf("BROKEN FMT next chip is %c\n", sv.items[i + 1]);
            return false;
        } else if (sv.items[i] == '}' && sv.items[i + 1] == '}') {
            i++;
            tmb_formatter_add_const_chip(formatter, sv.items + i, 1);
            continue;
        }
        if (sv.items[i] == '{' && sv.items[i + 1] == '{') {
            i++;
            tmb_formatter_add_const_chip(formatter, sv.items + i, 1);
            continue;
        }
        if (sv.items[i] == '{') {
            if (buff.length > 0) {
                tmb_formatter_add_const_chip(
                        formatter, buff.items, buff.length);
                sv_reset(&buff);
            }
            i++;

            while (sv.items[i] != '}') {
                if (i >= sv.length) {
                    printf("BROKEN FMT\n");
                    return false;
                }
                if (sv.items[i] == ':') {
                    tmb_string_view_t c = { .items  = buff.items,
                                            .length = buff.length };
                    da_append(&opts, c);
                    sv_reset(&buff);
                } else {
                    sv_init_or_extend(&buff, &sv, i);
                }
                i++;
            }
            if (!tmb_formatter_add_chip_from_opt(formatter, &buff, &opts)) {
                printf("BROKEN FMT\n");
                return false;
            }
            da_free(&opts);
            opts = (tmb_chip_opts_t) { 0 };
            sv_reset(&buff);

        } else {
            sv_init_or_extend(&buff, &sv, i);
        }
    }
    if (buff.length > 0) {
        tmb_formatter_add_const_chip(formatter, buff.items, buff.length);
    }
    formatter->data_free_fn     = do_nothing__;
    formatter->formated_free_fn = free;
    formatter->format_fn        = tmb_format;
    formatter->data             = NULL;
    da_free(&opts);
    return true;
}
