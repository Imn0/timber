#define TMB_LOGGING_IMPLEMENTATION
#define TMB_MIN_LOG_LEVEL 5
#include <tmb.h>

int main(void) {
    LOG_FATAL("message %d", 3);
    LOG_ERROR("message %d", 3);
    LOG_WARNING("message %d", 3);
    LOG_INFO("message %d", 3);
    LOG_DEBUG("message %d", 3);
    LOG_TRACE("message %d", 3);

    LOG_WARN("WAWD");
}
