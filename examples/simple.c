#include <tmb.h>

int main(int argc, char* argv[]) {
    Logger lg;
    tmb_logger_init_default(&lg);
    TMB_INFO(&lg, "wtf%d", 3);
}
