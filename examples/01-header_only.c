#define TMB_LOGGING_IMPLEMENTATION
#include <tmb/tmb.h>

int main(void) {
    // just use it
    LOG_FATAL("just a message");
    LOG_ERROR("a message with number %d", 3);
    LOG_WARNING("literrally just %s", "printf");
    LOG_INFO("message %d", 3);
    LOG_DEBUG("debug wont show by default");

    LOG_DEBUG("and now it will");

    // only global config does anything in header only mode
    TMB_CFG(.enable_colors   = false,
            .log_level       = TMB_LOG_LEVEL_WARNING,
            .default_sink_fd = 1);

    LOG_WARNING("this will be without color");
    LOG_INFO("and this wont show");

    TMB_CFG(.log_level = TMB_LOG_LEVEL_DEBUG, .enable_colors = true);
    LOG_DEBUG("and now it will");
}
