#include <tmb/tmb.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

int main() {
    tmb_logger_t* lgr = tmb_logger_create("d");
    tmb_logger_set_default_format(lgr, "{$BLUE:}{$}{$RED:}aa{$RESET:}\n");
    tmb_logger_add_sink(lgr, TMB_SINK_STDOUT());
    LOG_INFO(lgr, "hej");
}
