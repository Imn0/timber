#include <tmb.h>

volatile int a() {

    const char* a = GIT_REV;
    const char* b = GIT_TAG;
    const char* c = GIT_BRANCH;
    const char* d = GIT_PROJECT_VERSION;
    const char* e = PROJECT_VERSION;
    const char* f = SOVERSION;
    return 0;
}

int main(void) {
    a();
    return 0;
}
