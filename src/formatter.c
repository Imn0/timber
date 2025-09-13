#include <ctype.h>
#include <tmb_internal.h>

enum CHIP_TYPE {
    CHIP_MESSAGE,
    CHIP_CONST_VAL,
    CHIP_LEVEL_L,
    CHIP_LEVEL_S,
    CHIP_BASEFILE,
    CHIP_FILE,
    CHIP_LINE,
    CHIP_FUNC,
};

typedef struct tmb_chip {
    enum CHIP_TYPE type;
    union {
        tmb_string_view_t const_val;
    };
    int just_amount;
    enum tmb_sb_just_opt just_opt;
    enum tmb_sb_truncate_opt truncate_opt;
} tmb_chip_t;

static inline void tmb_chip_format(tmb_chip_t* chip,
                                   tmb_string_builder_t* target,
                                   const tmb_log_ctx_t* const ctx,
                                   bool use_color) {
    tmb_string_builder_t buff       = { 0 };
    tmb_string_builder_t color_buff = { 0 };
    switch (chip->type) {
    case CHIP_MESSAGE:
        sb_appendn(&buff, ctx->message, ctx->message_len);
        break;
    case CHIP_CONST_VAL:
        sb_appendn(&buff, chip->const_val.items, chip->const_val.length);
        break;
    case CHIP_LEVEL_L:
        if (use_color) {
            sb_appendn(&color_buff,
                       tmb_log_level_color[ctx->log_level],
                       tmb_log_level_color_len[ctx->log_level]);
        }
        sb_appendn(&buff,
                   tmb_log_level_str[ctx->log_level],
                   tmb_log_level_str_len[ctx->log_level]);
        break;
    case CHIP_LEVEL_S:
        if (use_color) {
            sb_appendn(&color_buff,
                       tmb_log_level_color[ctx->log_level],
                       tmb_log_level_color_len[ctx->log_level]);
        }
        sb_append(&buff, tmb_log_level_char[ctx->log_level]);
        break;
    case CHIP_BASEFILE:
        sb_appendf(&buff, "%.*s", ctx->filename_base_len, ctx->filename_base);
        break;
    case CHIP_FILE:
        sb_appendf(&buff, "%.*s", ctx->filename_len, ctx->filename);
        break;
    case CHIP_LINE:
        sb_appendf(&buff, "%d", ctx->line_no);
        break;
    case CHIP_FUNC:
        sb_appendf(&buff, "%.*s", ctx->funcname_len, ctx->funcname);
        break;
    default:
        break;
    }
    if (buff.length < chip->just_amount) {
        tmb_sb_just(&buff, chip->just_opt, chip->just_amount, ' ');
    } else if (buff.length > chip->just_amount) {
        tmb_sb_truncate(&buff, chip->truncate_opt, chip->just_amount);
    }
    if (use_color) {
        sb_appendn(&color_buff, buff.items, buff.length);
        sb_appendn(&color_buff,
                   MAKE_ANSI(ANSI_RESET),
                   (int)TMB_CONST_STR_SIZE(MAKE_ANSI(ANSI_RESET)));
        sb_appendn(target, color_buff.items, color_buff.length);
        sb_free(&buff);
        sb_free(&color_buff);
    } else {
        sb_appendn(target, buff.items, buff.length);
    }
    sb_free(&buff);
}

static bool tmb_chip_init_from_opt(tmb_chip_t* chip,
                                   char fmt_opt,
                                   char truncate_opt_chr,
                                   char just_opt_chr,
                                   int just_amount) {
#define CASE(c, new_type)                                                      \
    case c:                                                                    \
        chip->type = new_type;                                                 \
        break

    switch (fmt_opt) {
        CASE('$', CHIP_MESSAGE);
        CASE('l', CHIP_LEVEL_L);
        CASE('L', CHIP_LEVEL_S);
        CASE('@', CHIP_BASEFILE);
        CASE('s', CHIP_FILE);
        CASE('#', CHIP_LINE);
        CASE('f', CHIP_FUNC);
    default:
        UNUSED fprintf(stderr, "unknown format %c\n", fmt_opt);
        return false;
        break;
    }
    enum tmb_sb_truncate_opt truncate_opt = TRUNCATE_OFF;
    enum tmb_sb_just_opt just_opt         = JUST_OFF;
    switch (truncate_opt_chr) {
    case TRUNCATING_LEFT_OPT_CHAR:
        truncate_opt = TRUNCATE_LEFT;
        break;
    case TRUNCATING_RIGHT_OPT_CHAR:
        truncate_opt = TRUNCATE_RIGHT;
        break;
    default:
        break;
    }

    switch (just_opt_chr) {
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
        break;
    }
    chip->just_amount  = just_amount;
    chip->truncate_opt = truncate_opt;
    chip->just_opt     = just_opt;
    return true;
}

static bool tmb_chip_init_const(tmb_chip_t* chip,
                                const char* val,
                                int val_len) {
    chip->type        = CHIP_CONST_VAL;
    chip->just_amount = JUST_OFF;

    tmb_string_builder_t sb = { 0 };
    sb_appendn(&sb, val, val_len);

    chip->const_val = sv_from_sb(&sb);
    return true;
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

bool tmb_formatter_init(tmb_formatter_t* formatter, const char* fmt) {
    tmb_string_builder_t sb = { 0 };
    int fmt_len             = (int)strlen(fmt);

    for (int i = 0; i < fmt_len; i++) {
        if (fmt[i] == '%') {
            if (sb.length > 0) {
                da_append(formatter, (tmb_chip_t) { 0 });
                tmb_chip_init_const(&formatter->items[formatter->length - 1],
                                    sb.items,
                                    sb.length);
                sb_free(&sb);
            }
            i++;
            char just_opt     = 0;
            char truncate_opt = 0;
            int just_amount   = 0;
            if (isdigit(fmt[i])) {
                while (isdigit(fmt[i])) {
                    just_amount *= 10;
                    just_amount += (fmt[i] - '0');
                    i++;
                }
            }
            if (fmt[i] == JUSTING_LEFT_OPT_CHAR ||
                fmt[i] == JUSTING_CENTER_OPT_CHAR ||
                fmt[i] == JUSTING_RIGHT_OPT_CHAR) {
                just_opt = fmt[i++];
            }
            if (fmt[i] == TRUNCATING_LEFT_OPT_CHAR ||
                fmt[i] == TRUNCATING_RIGHT_OPT_CHAR) {
                truncate_opt = fmt[i++];
            }
            da_append(formatter, (tmb_chip_t) { 0 });
            if (!tmb_chip_init_from_opt(
                        &formatter->items[formatter->length - 1],
                        fmt[i],
                        truncate_opt,
                        just_opt,
                        just_amount)) {
                return false;
            }
        } else {
            sb_append(&sb, fmt[i]);
        }
    }
    if (sb.length > 0) {
        da_append(formatter, (tmb_chip_t) { 0 });
        tmb_chip_init_const(
                &formatter->items[formatter->length - 1], sb.items, sb.length);
        sb_free(&sb);
    }

    formatter->data_free_fn     = do_nothing__;
    formatter->formated_free_fn = free;
    formatter->format_fn        = tmb_format;
    formatter->data             = NULL;

    return true;
}
