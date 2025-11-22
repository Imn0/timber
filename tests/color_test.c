#include <sink_internal.h>
#include <tmb/tmb.h>
#include <tmb_internal.h>

int main() {
    tmb_logger_t* lgr = TMB_LOGGER("d");
    tmb_logger_set_default_format(lgr, "{$BLUE:}{$}{$RED:}aa{$RESET:}\n");
    int sink_idx  = tmb_logger_add_sink(lgr, tmb_sink_buffer_create(1024));
    tmb_sink_t* s = lgr->sinks.items[sink_idx];
    LOG_INFO(lgr, "hej");

    struct sink_buffer_data* d = s->sink_data;
    tmb_string_view_t sv1      = { .items = d->items, .length = d->length };

    ASSERT(tmb_sv_cmp(&sv1, &sv_make("\033[34mhej\033[31maa\033[0m\n")));
}
