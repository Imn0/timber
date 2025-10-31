#include <stdlib.h>
#include <tmb/sink.h>
#include <tmb/tmb.h>

struct sink_data {
    int a;
};

void custom_sink_fn(const char* message, int message_length, void* data) {
    struct sink_data* sink_data = data;
    printf("custom sink has been called %d times -> %.*s",
           ++sink_data->a,
           message_length,
           message);
}

int main(void) {
    tmb_logger_t* lgr      = TMB_LOGGER("my logger");
    struct sink_data* data = malloc(sizeof(*data));
    tmb_sink_t sink        = { .sink_fn   = custom_sink_fn,
                               .free_fn   = free,
                               .sink_data = data };
    tmb_logger_add_sink(lgr, &sink);
    tmb_logger_set_default_format(lgr, "{$}\n");
    LOG_WARNING(lgr, "hello");
    LOG_WARNING(lgr, "hello");
}
