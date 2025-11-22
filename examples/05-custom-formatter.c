#include <string.h>
#include <tmb/sink.h>
#include <tmb/tmb.h>

void pass(void* thing) {
    (void)thing;
}

tmb_formatted_msg_t format(const tmb_formatter_t* formatter,
                           const tmb_log_ctx_t* const ctx,
                           const tmb_logger_t* lgr) {
    (void)formatter;
    (void)lgr;
    static char buff[1024];
    snprintf(buff, 1024, "message -> %s\n", ctx->message);

    return (tmb_formatted_msg_t) { .message = buff, .length = (int)strlen(buff) };
}

int main(void) {
    tmb_logger_t* lgr = TMB_LOGGER("my logger");

    tmb_formatter_t formatter = { .format_fn        = format,
                                  .formated_free_fn = pass,
                                  .data_free_fn     = pass };

    tmb_logger_add_sink(lgr, TMB_SINK_STDOUT());
    tmb_logger_add_formatter(lgr, formatter);
    LOG_WARNING(lgr, "hello");
    LOG_WARNING(lgr, "hello");
}
