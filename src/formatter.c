#include <ctype.h>
#include <formatter.h>

static inline void handle_color_chip(tmb_chip_t* chip,
                                     tmb_string_builder_t* target) {
    switch (chip->ansi_val) {
    case CHIP_ANSI_RESET:
        sb_append_cstr(target, MAKE_ANSI(ANSI_RESET));
        break;
    case CHIP_ANSI_BOLD:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BOLD));
        break;
    case CHIP_ANSI_DIM:
        sb_append_cstr(target, MAKE_ANSI(ANSI_DIM));
        break;
    case CHIP_ANSI_ITALIC:
        sb_append_cstr(target, MAKE_ANSI(ANSI_ITALIC));
        break;
    case CHIP_ANSI_UNDERLINE:
        sb_append_cstr(target, MAKE_ANSI(ANSI_UNDERLINE));
        break;
    case CHIP_ANSI_BLINK:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BLINK));
        break;
    case CHIP_ANSI_REVERSE:
        sb_append_cstr(target, MAKE_ANSI(ANSI_REVERSE));
        break;
    case CHIP_ANSI_BLACK:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BLACK));
        break;
    case CHIP_ANSI_RED:
        sb_append_cstr(target, MAKE_ANSI(ANSI_RED));
        break;
    case CHIP_ANSI_GREEN:
        sb_append_cstr(target, MAKE_ANSI(ANSI_GREEN));
        break;
    case CHIP_ANSI_YELLOW:
        sb_append_cstr(target, MAKE_ANSI(ANSI_YELLOW));
        break;
    case CHIP_ANSI_BLUE:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BLUE));
        break;
    case CHIP_ANSI_MAGENTA:
        sb_append_cstr(target, MAKE_ANSI(ANSI_MAGENTA));
        break;
    case CHIP_ANSI_CYAN:
        sb_append_cstr(target, MAKE_ANSI(ANSI_CYAN));
        break;
    case CHIP_ANSI_WHITE:
        sb_append_cstr(target, MAKE_ANSI(ANSI_WHITE));
        break;
    case CHIP_ANSI_BG_BLACK:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_BLACK));
        break;
    case CHIP_ANSI_BG_RED:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_RED));
        break;
    case CHIP_ANSI_BG_GREEN:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_GREEN));
        break;
    case CHIP_ANSI_BG_YELLOW:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_YELLOW));
        break;
    case CHIP_ANSI_BG_BLUE:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_BLUE));
        break;
    case CHIP_ANSI_BG_MAGENTA:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_MAGENTA));
        break;
    case CHIP_ANSI_BG_CYAN:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_CYAN));
        break;
    case CHIP_ANSI_BG_WHITE:
        sb_append_cstr(target, MAKE_ANSI(ANSI_BG_WHITE));
        break;
    case CHIP_ANSI_INVALID:
    default:
        sb_append_cstr(target, MAKE_ANSI(ANSI_RESET));
    }
}

static inline void tmb_chip_format(tmb_chip_t* chip,
                                   tmb_string_builder_t* target,
                                   const tmb_log_ctx_t* const ctx,
                                   const tmb_logger_t* lgr,
                                   const tmb_formatter_t* fmt) {
    bool use_color = fmt->enable.colors && lgr->cfg.enable_colors &&
                     tmb_cfg.enable_colors;

    bool auto_color_used = false;

    if (use_color) {
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wswitch-enum"
#endif
        switch (chip->type) {
        case CHIP_TYPE_LEVEL_L:
        case CHIP_TYPE_LEVEL_S:
            sb_appendn(target,
                       tmb_log_level_color_len[ctx->log_level],
                       tmb_log_level_color[ctx->log_level]);
            auto_color_used = true;
            break;
        default:
            break;
        }
    }
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

    int content_start = target->length;
    switch (chip->type) {
    case CHIP_TYPE_UNKNOWN:
        printf("ERROR CHIP\n");
        break;
    case CHIP_TYPE_MESSAGE:
        sb_appendn(target, ctx->message_len, ctx->message);
        break;
    case CHIP_TYPE_CONST_VAL:
        sb_appendn(target, chip->const_val.length, chip->const_val.items);
        break;
    case CHIP_TYPE_LEVEL_L:

        sb_appendn(target,
                   tmb_log_level_str_len[ctx->log_level],
                   tmb_log_level_str[ctx->log_level]);
        break;
    case CHIP_TYPE_LEVEL_S:
        sb_append(target, tmb_log_level_char[ctx->log_level]);
        break;
    case CHIP_TYPE_BASEFILE:
        sb_appendn(target, ctx->filename_base_len, ctx->filename_base);
        break;
    case CHIP_TYPE_FILE:
        sb_appendn(target, ctx->filename_len, ctx->filename);
        break;
    case CHIP_TYPE_LINE:
        sb_appendf(target, "%d", ctx->line_no);
        break;
    case CHIP_TYPE_FUNC:
        sb_appendn(target, ctx->funcname_len, ctx->funcname);
        break;
    case CHIP_TYPE_LOGGER_NAME:
        sb_appendn(target, (int)strlen(lgr->name), lgr->name);
        break;
    case CHIP_TYPE_TAG:
        if (lgr->tags.length > 0) {
            sb_appendn(target,
                       lgr->tags.items[lgr->tags.length - 1].length,
                       lgr->tags.items[lgr->tags.length - 1].items);
        }
        break;
    case CHIP_TYPE_TAGS:
        for (int i = 0; i < lgr->tags.length; i++) {
            sb_appendn(target,
                       lgr->tags.items[i].length,
                       lgr->tags.items[i].items);
            if (i != lgr->tags.length - 1) { sb_append(target, ':'); }
        }
        break;
    case CHIP_TYPE_LOGGER_STOPWATCH:
        float d = tmb_timestamp_diff(
                (tmb_timestamp_t) { .sec  = ctx->stopwatch_sec,
                                    .nsec = ctx->stopwatch_nsec },
                (tmb_timestamp_t) { .sec  = lgr->last_message_stopwatch_sec,
                                    .nsec = lgr->last_message_stopwatch_nsec });
        sb_appendf(target, "%f", (double)d);
        break;
    case CHIP_TYPE_LOGGER_TIMESTAMP:
        // Convert to local time
        {
            struct tm* tm_info;
            time_t secs = (time_t)ctx->ts_sec;
            tm_info     = localtime(&secs);

            // Format the time string with nanoseconds
            // Example: "2024-01-15 14:30:45.123456789"
            da_reserve(target, target->length + 40);
            size_t written = strftime(
                    target->items + target->length,
                    (size_t)(target->capacity - target->length),
                    "%Y-%m-%d %H:%M:%S",
                    tm_info);
            target->length += (int)written;
            sb_appendf(target, ".%09lld", ctx->ts_nsec);
        }
        break;
    case CHIP_TYPE_COLOR:
        if (use_color) { handle_color_chip(chip, target); }
        break;
    case CHIP_TYPE_LOG_LEVEL_COLOR:
        if (use_color) {
            sb_appendn(target,
                       tmb_log_level_color_len[ctx->log_level],
                       tmb_log_level_color[ctx->log_level]);
        }
        break;
    default:
        break;
    }
    int content_len = target->length - content_start;

    if (chip->just_amount > 0) {
        if (content_len < chip->just_amount) {
            tmb_sb_just_inplace(target,
                                content_start,
                                content_len,
                                chip->just_opt,
                                chip->just_amount,
                                ' ');
        } else if (content_len > chip->just_amount) {
            tmb_sb_truncate_inplace(target,
                                    content_start,
                                    content_len,
                                    chip->truncate_opt,
                                    chip->just_amount);
        }
    }
    if (auto_color_used) {
        sb_appendn(target,
                   (int)TMB_CONST_STR_SIZE(MAKE_ANSI(ANSI_RESET)),
                   MAKE_ANSI(ANSI_RESET));
    }
}

static tmb_formatted_msg_t tmb_format(const tmb_formatter_t* formatter,
                                      const tmb_log_ctx_t* const ctx,
                                      const tmb_logger_t* lgr) {
    tmb_string_builder_t message = { 0 };
    da_reserve(&message, ctx->message_len + 5 * formatter->length);
    for (int i = 0; i < formatter->length; i++) {
        tmb_chip_format(&formatter->items[i], &message, ctx, lgr, formatter);
    }
    return (tmb_formatted_msg_t) { .items  = message.items,
                                   .length = message.length };
}

static enum CHIP_ANSI chip_ansi_from_str(tmb_string_view_t sv) {
    if (tmb_is_substring(sv, "BOLD")) { return CHIP_ANSI_BOLD; }
    if (tmb_is_substring(sv, "DIM")) { return CHIP_ANSI_DIM; }
    if (tmb_is_substring(sv, "ITALIC")) { return CHIP_ANSI_ITALIC; }
    if (tmb_is_substring(sv, "UNDERLINE")) { return CHIP_ANSI_UNDERLINE; }
    if (tmb_is_substring(sv, "BLINK")) { return CHIP_ANSI_BLINK; }
    if (tmb_is_substring(sv, "REVERSE")) { return CHIP_ANSI_REVERSE; }
    if (tmb_is_substring(sv, "BLACK")) { return CHIP_ANSI_BLACK; }
    if (tmb_is_substring(sv, "RED")) { return CHIP_ANSI_RED; }
    if (tmb_is_substring(sv, "GREEN")) { return CHIP_ANSI_GREEN; }
    if (tmb_is_substring(sv, "YELLOW")) { return CHIP_ANSI_YELLOW; }
    if (tmb_is_substring(sv, "BLUE")) { return CHIP_ANSI_BLUE; }
    if (tmb_is_substring(sv, "MAGENTA")) { return CHIP_ANSI_MAGENTA; }
    if (tmb_is_substring(sv, "CYAN")) { return CHIP_ANSI_CYAN; }
    if (tmb_is_substring(sv, "WHITE")) { return CHIP_ANSI_WHITE; }
    if (tmb_is_substring(sv, "BG_BLACK")) { return CHIP_ANSI_BG_BLACK; }
    if (tmb_is_substring(sv, "BG_RED")) { return CHIP_ANSI_BG_RED; }
    if (tmb_is_substring(sv, "BG_GREEN")) { return CHIP_ANSI_BG_GREEN; }
    if (tmb_is_substring(sv, "BG_YELLOW")) { return CHIP_ANSI_BG_YELLOW; }
    if (tmb_is_substring(sv, "BG_BLUE")) { return CHIP_ANSI_BG_BLUE; }
    if (tmb_is_substring(sv, "BG_MAGENTA")) { return CHIP_ANSI_BG_MAGENTA; }
    if (tmb_is_substring(sv, "BG_CYAN")) { return CHIP_ANSI_BG_CYAN; }
    if (tmb_is_substring(sv, "BG_WHITE")) { return CHIP_ANSI_BG_WHITE; }
    return CHIP_ANSI_INVALID;
}

typedef struct tmb_chip_opts {
    _da_header_(tmb_string_view_t*);
} tmb_chip_opts_t;

static bool tmb_formatter_add_chip_from_opt(tmb_formatter_t* formatter,
                                            tmb_string_view_t* chip_type,
                                            tmb_chip_opts_t* opts) {
    tmb_chip_t chip = { 0 };

    bool auto_reset_color = false;
    bool doing_color      = false;

    for (int i = 0; i < opts->length; i++) {
        tmb_string_view_t* current_opt = &opts->items[i];
        if (current_opt->length <= 0) { continue; }

        // when we have color as an additional option, not as the main thing
        if (current_opt->items[0] == '$') {
            doing_color = true;
            if (tmb_sv_cmp(&sv_make("$LEVEL"), current_opt)) {
                tmb_chip_t color_chip = { .type = CHIP_TYPE_LOG_LEVEL_COLOR };
                da_append(formatter, color_chip);
            } else {
                tmb_chip_t color_chip = { .type = CHIP_TYPE_COLOR };
                color_chip.ansi_val   = chip_ansi_from_str(*current_opt);
                da_append(formatter, color_chip);
            }
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
                    chip.just_opt = JUST_LEFT;
                    break;
                case JUSTING_CENTER_OPT_CHAR:
                    chip.just_opt = JUST_CENTER;
                    break;
                case JUSTING_RIGHT_OPT_CHAR:
                    chip.just_opt = JUST_RIGHT;
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
                    chip.truncate_opt = TRUNCATE_LEFT;
                    break;
                case TRUNCATING_RIGHT_OPT_CHAR:
                    chip.truncate_opt = TRUNCATE_RIGHT;
                    break;
                default:
                    unreachable();
                    break;
                }
            }
            if (isdigit(current_opt_char)) {
                chip.just_amount *= 10;
                chip.just_amount += (current_opt_char - '0');
            }

            j++;
        }
    }

#define CASE(c, new_type)                                                      \
    case c:                                                                    \
        chip.type = new_type;                                                  \
        break

    if (chip_type->length == 1) {
        auto_reset_color = true;
        switch (chip_type->items[0]) {
            CASE('$', CHIP_TYPE_MESSAGE);
            CASE('l', CHIP_TYPE_LEVEL_L);
            CASE('L', CHIP_TYPE_LEVEL_S);
            CASE('@', CHIP_TYPE_BASEFILE);
            CASE('s', CHIP_TYPE_FILE);
            CASE('#', CHIP_TYPE_LINE);
            CASE('f', CHIP_TYPE_FUNC);
            CASE('t', CHIP_TYPE_TAG);
            CASE('T', CHIP_TYPE_TAGS);
            CASE('n', CHIP_TYPE_LOGGER_NAME);
        case 'd':
            chip.type                = CHIP_TYPE_LOGGER_STOPWATCH;
            formatter->has.stopwatch = true;
            break;
        case 'D':
            chip.type                = CHIP_TYPE_LOGGER_TIMESTAMP;
            formatter->has.timestamp = true;
            break;
        default:
            UNUSED fprintf(stderr, "unknown format %c\n", chip_type->items[0]);
            return false;
            break;
        }
    } else if (chip_type->length > 1 && chip_type->items[0] == '$') {
        // when $<COLOR> is the only thing in format chip
        if (tmb_sv_cmp(&sv_make("$LEVEL"), chip_type)) {
            chip.type = CHIP_TYPE_LOG_LEVEL_COLOR;
        } else {
            chip.type     = CHIP_TYPE_COLOR;
            chip.ansi_val = chip_ansi_from_str(*chip_type);
        }

    } else if (chip_type->length == 0) {
        goto skip_append;
        // idk do nothing
    }
    da_append(formatter, chip);

skip_append:
    if (doing_color && auto_reset_color) {
        tmb_chip_t r_chip = { .type     = CHIP_TYPE_COLOR,
                              .ansi_val = CHIP_ANSI_RESET };
        da_append(formatter, r_chip);
    }

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
        if (sv.items[i] == '}' && i + 1 < sv.length && sv.items[i + 1] != '}') {
            printf("BROKEN FMT next chip is %c\n", sv.items[i + 1]);
            goto fail;
        } else if (i + 1 < sv.length && sv.items[i] == '}' &&
                   sv.items[i + 1] == '}') {
            // flush any buffered
            if (buff.length > 0) {
                tmb_formatter_add_const_chip(
                        formatter, buff.items, buff.length);
                sv_reset(&buff);
            }
            i++; // skip the second }
            tmb_formatter_add_const_chip(formatter, "}", 1);
            continue;
        }
        if (i + 1 < sv.length && sv.items[i] == '{' && sv.items[i + 1] == '{') {
            // flush any buffered
            if (buff.length > 0) {
                tmb_formatter_add_const_chip(
                        formatter, buff.items, buff.length);
                sv_reset(&buff);
            }
            i++; // skip the second {
            tmb_formatter_add_const_chip(formatter, "{", 1);
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
                    goto fail;
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
                goto fail;
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
    formatter->enable.colors    = true;
    da_free(&opts);
    return true;
fail:
    da_free(&opts);
    da_free(formatter);
    return false;
}

void tmb_formatter_print(const tmb_formatter_t* formatter) {
    printf("formatter:\n");
    for (int i = 0; i < formatter->length; i++) {
        printf("chip :");
        switch (formatter->items[i].type) {
        case CHIP_TYPE_MESSAGE:
            printf("CHIP_TYPE_MESSAGE");
            break;
        case CHIP_TYPE_CONST_VAL:
            printf("CHIP_TYPE_CONST_VAL");
            printf(" >%.*s<",
                   formatter->items[i].const_val.length,
                   formatter->items[i].const_val.items);
            break;
        case CHIP_TYPE_LEVEL_L:
            printf("CHIP_TYPE_LEVEL_L");
            break;
        case CHIP_TYPE_LEVEL_S:
            printf("CHIP_TYPE_LEVEL_S");
            break;
        case CHIP_TYPE_BASEFILE:
            printf("CHIP_TYPE_BASEFILE");
            break;
        case CHIP_TYPE_FILE:
            printf("CHIP_TYPE_FILE");
            break;
        case CHIP_TYPE_LINE:
            printf("CHIP_TYPE_LINE");
            break;
        case CHIP_TYPE_FUNC:
            printf("CHIP_TYPE_FUNC");
            break;
        case CHIP_TYPE_COLOR:
            printf("CHIP_TYPE_COLOR ");
            printf("%d ", formatter->items[i].ansi_val);
            break;
        case CHIP_TYPE_TAG:
            printf("CHIP_TYPE_TAG");
            break;
        case CHIP_TYPE_TAGS:
            printf("CHIP_TYPE_TAGS");
            break;
        case CHIP_TYPE_LOGGER_NAME:
            printf("CHIP_TYPE_LOGGER_NAME");
            break;
        case CHIP_TYPE_LOGGER_STOPWATCH:
            printf("CHIP_TYPE_LOGGER_STOPWATCH");
            break;
        case CHIP_TYPE_LOGGER_TIMESTAMP:
            printf("CHIP_TYPE_LOGGER_TIMESTAMP");
            break;
        case CHIP_TYPE_LOG_LEVEL_COLOR:
            printf("CHIP_TYPE_LOG_LEVEL_COLOR");
            break;
        default:
        case CHIP_TYPE_UNKNOWN:
            printf("CHIP_TYPE_UNKNOWN");
            break;
        }
        printf("\n");
    }
}

TMB_API tmb_formatter_t tmb_formatter_make(const char* fmt) {
    tmb_formatter_t formatter = { 0 };
    if (!tmb_formatter_init(&formatter, fmt)) {
        return (tmb_formatter_t) { 0 };
    }
    return formatter;
}

void tmb_formatter_deinit(tmb_formatter_t* formatter) {
    if (formatter->data_free_fn != NULL) {
        formatter->data_free_fn(formatter->data);
    }
    for (int j = 0; j < formatter->length; j++) {
        tmb_chip_t* chip = &formatter->items[j];
        if (chip->type == CHIP_TYPE_CONST_VAL) {
            free((void*)chip->const_val.items);
        }
    }
}
