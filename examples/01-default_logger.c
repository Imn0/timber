#include <tmb/tmb.h>

int main(void) {
    // just use it
    LOG_FATAL("just a message");
    LOG_ERROR("a message with number %d", 3);
    LOG_WARNING("literrally just %s", "printf");
    LOG_INFO("message %d", 3);
    LOG_DEBUG("message %d", 3);
}
