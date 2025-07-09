#define TMB_MIN_LOG_LEVEL 7
#include <tmb.h>

int main(void) {
    TMB_EMERGENCY("message %d", 3);
    TMB_ALERT("message %d", 3);
    TMB_CRITICAL("message %d", 3);
    TMB_ERROR("message %d", 3);
    TMB_WARNING("message %d", 3);
    TMB_NOTICE("message %d", 3);
    TMB_INFO("message %d", 3);
    TMB_DEBUG("message %d", 3);
}
