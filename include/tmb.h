#ifndef TMB_LIB
#define TMB_LIB
#include <stdbool.h>

extern const char* GIT_REV;
extern const char* GIT_TAG;
extern const char* GIT_BRANCH;
extern const char* GIT_PROJECT_VERSION;
extern const char* PROJECT_VERSION;
extern const char* SOVERSION;

// formater f type
#define TMB_LOG(logger, log_level, ...)                                        \
    do {                                                                       \
        __tmb_log(logger,                                                      \
                  log_level,                                                   \
                  __LINE__,                                                    \
                  __FILE__,                                                    \
                  __func__,                                                    \
                  __VA_ARGS__);                                                \
    } while (0)

bool tmb_init(const char* init_string);

extern void __tmb_log(void* logger,
                      int log_level,
                      int line_no,
                      const char* filename,
                      const char* funcname,
                      const char* message,
                      ...);

#endif
