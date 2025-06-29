#ifndef TMB_H
#define TMB_H
#include <stdbool.h>

extern const char* GIT_REV;
extern const char* GIT_TAG;
extern const char* GIT_BRANCH;
extern const char* GIT_PROJECT_VERSION;
extern const char* PROJECT_VERSION;
extern const char* SOVERSION;




typedef struct {
    int a;
    char** consts;
} Logger;

typedef enum {
    TMB_TRACE = 0,
    TMB_DEBUG,
    TMB_INFO,
    TMB_NOTICE,
    TMB_WARNING,
    TMB_ERROR,
    TMB_ALERT,
    TMB_CRITICAL = 9,
} tmb_log_level;

typedef struct {
    int line_no;
    const char* filename;
    const char* funcname;
} LogCtx;

#define TMB_LOG(logger, log_level, ...)                                        \
    __tmb_log(logger,                                                          \
              log_level,                                                       \
              (LogCtx) { __LINE__, __FILE__, __func__ },                       \
              __VA_ARGS__)

bool tmb_init(const char* init_string);

void __tmb_log(void* logger,
               tmb_log_level log_level,
               LogCtx ctx,
               const char* message,
               ...);

void tmb_test();

#endif
