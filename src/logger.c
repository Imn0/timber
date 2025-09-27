#include <formatter.h>
#include <tmb_internal.h>

#include <tmb/sink.h>

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

tmb_logger_t* tmb_logger_create(const char* logger_name, tmb_cfg_t cfg) {
    tmb_logger_t* lgr = malloc(sizeof(*lgr));
    memset(lgr, 0, sizeof(*lgr));

    lgr->cfg  = cfg;
    int logger_name_len = (int)strlen(logger_name);
    int len_to_cpy      = 0;
    if (logger_name_len > MAX_LOGGER_NAME_LEN) {
        len_to_cpy = MAX_LOGGER_NAME_LEN;
    } else {
        len_to_cpy = logger_name_len;
    }

    for (int i = 0; i < len_to_cpy; i++) { lgr->name[i] = logger_name[i]; }

    return lgr;
}

TMB_API void tmb_logger_destroy(tmb_logger_t* logger) {
    for (int i = 0; i < logger->sinks.length; i++) {
        tmb_sink_t* sink = &logger->sinks.items[i];
        tmb_sink_deinit(sink);
    }

    for (int i = 0; i < logger->formatters.length; i++) {
        tmb_formatter_t* fmt = &logger->formatters.items[i];
        tmb_formatter_deinit(fmt);
        da_free(fmt);
    }

    da_free(&logger->formatters);
    da_free(&logger->sinks);
    da_free(&logger->sink_formatter_map);
    free(logger);
    logger = NULL;
}

bool tmb_logger_set_default_format(tmb_logger_t* logger, const char* fmt) {
    if (logger->formatters.length == 0) {
        da_append(&logger->formatters, (tmb_formatter_t) { 0 });
    } else {
        da_free(&logger->formatters.items[0]);
    }
    return tmb_formatter_init(&logger->formatters.items[0], fmt);
}

int tmb_logger_add_sink(tmb_logger_t* logger, tmb_sink_t sink) {
    da_append(&logger->sinks, sink);
    da_append(&logger->sink_formatter_map, 0);
    return logger->sinks.length - 1;
}

int tmb_logger_add_formatter(tmb_logger_t* lgr, tmb_formatter_t formatter) {
    da_append(&lgr->formatters, formatter);
    return lgr->formatters.length - 1;
}

int tmb_logger_add_format(tmb_logger_t* lgr, const char* fmt) {
    tmb_formatter_t formatter = { 0 };
    if (tmb_formatter_init(&formatter, fmt)) {
        da_append(&lgr->formatters, formatter);
        return lgr->formatters.length - 1;
    } else {
        da_free(&formatter);
        return -1;
    }
}

int tmb_logger_assign_format(tmb_logger_t* lgr, int sink_idx, int fmt_idx) {
    if (sink_idx >= lgr->sinks.length ||
        sink_idx >= lgr->sink_formatter_map.length) {
        return -1;
    }
    if (fmt_idx >= lgr->formatters.length) { return -2; }
    lgr->sink_formatter_map.items[sink_idx] = fmt_idx;
    return 1;
}

int tmb_logger_set_format(tmb_logger_t* lgr, int sink_idx, const char* fmt) {
    if (sink_idx >= lgr->sinks.length ||
        sink_idx >= lgr->sink_formatter_map.length) {
        return -1;
    }
    tmb_formatter_t formatter = { 0 };
    if (!tmb_formatter_init(&formatter, fmt)) { return -2; }
    da_append(&lgr->formatters, formatter);
    lgr->sink_formatter_map.items[sink_idx] = lgr->formatters.length - 1;
    return -1;
}

void tmb_logger_add_tag(tmb_logger_t* lgr, const char* tag) {
    tmb_string_view_t n_tag = { .items = tag, .length = (int)strlen(tag) };
    for (int i = 0; i < lgr->tags.length; i++) {
        tmb_string_view_t c_tag = { .length = lgr->tags.items[i].length,
                                    .items  = lgr->tags.items[i].items };
        if (tmb_sv_cmp(&n_tag, &c_tag)) { return; }
    }
    tmb_tag_t to_append = { .length = n_tag.length, .items = tmb_strdup(tag) };
    da_append(&lgr->tags, to_append);
}

void tmb_logger_remove_tag(tmb_logger_t* lgr, const char* tag) {
    tmb_string_view_t n_tag = { .items = tag, .length = (int)strlen(tag) };
    for (int i = 0; i < lgr->tags.length; i++) {
        tmb_string_view_t c_tag = { .length = lgr->tags.items[i].length,
                                    .items  = lgr->tags.items[i].items };
        if (tmb_sv_cmp(&n_tag, &c_tag)) {
            free((void*)c_tag.items);
            da_remove(&lgr->tags, i);
            return;
        }
    }
}
