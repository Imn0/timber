#include <tmb/sink.h>
#include <tmb/tmb.h>

void custom_sink_fn(const char* message, int message_length, void* data) {
    printf("custom sink -> %.*s", message_length, message);
}

int main(void) {
    tmb_logger_t lgr = { .log_level = TMB_LOG_LEVEL_INFO };
    tmb_sink_t sink  = { .sink_fn = custom_sink_fn };
    tmb_logger_add_sink(&lgr, sink);
    tmb_logger_set_default_format(&lgr, "%$\n");
    LOG_WARNING(&lgr, "hello");
}
