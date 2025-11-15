#include <tmb_internal.h>

#ifndef TMB_GRAYLOG_BUFF_SIZE
    #define TMB_GRAYLOG_BUFF_SIZE 1024
// https://go2docs.graylog.org/current/getting_in_log_data/gelf.html
#endif

static tmb_formatted_msg_t graylog_format(const tmb_formatter_t* formatter,
                                          const tmb_log_ctx_t* const ctx,
                                          const tmb_logger_t* lgr) {
    UNUSED formatter;
    static char buff[TMB_GRAYLOG_BUFF_SIZE];
    int written = snprintf(buff,
                           TMB_GRAYLOG_BUFF_SIZE,
                           "{"
                           "\"version\": \"1.1\","
                           "\"host\": \"%s\","
                           "\"short_message\": \"%s\","
                           "\"timestamp\": %lld.%lld,"
                           "\"level\": %d,"
                           "\"line\": %d,"
                           "\"file\": \"%s\","
                           "\"_logger\": \"%s\" "
                           "}\n",
                           lgr->name,
                           ctx->message,
                           ctx->ts_sec,
                           NSEC_TO_MSEC(ctx->ts_nsec),
                           ctx->log_level,
                           ctx->line_no,
                           ctx->filename,
                           lgr->name);
    if (written < 0 || written >= TMB_GRAYLOG_BUFF_SIZE) {
        int w = snprintf(buff,
                         TMB_GRAYLOG_BUFF_SIZE,
                         "{"
                         "\"short_message\": \"truncated message\","
                         "\"file\": \"%.50s\","
                         "\"line\": %d,"
                         "}\n",
                         ctx->filename,
                         ctx->line_no);
        return (tmb_formatted_msg_t) { .message = buff, .length = w };
    }
    return (tmb_formatted_msg_t) { .message = buff, .length = written };
}

tmb_formatter_t tmb_formatter_graylog_make(void) {
    tmb_formatter_t fmt = { .format_fn        = graylog_format,
                            .formated_free_fn = do_nothing__,
                            .data_free_fn     = do_nothing__,
                            .has.stopwatch    = false,
                            .has.timestamp   = true };
    return fmt;
}
