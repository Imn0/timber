#ifndef TMB_SINK_H
#define TMB_SINK_H

#include <tmb_lib.h>

typedef enum {
    STDERR_SINK
} SINK_TYPE;

typedef struct {
    sink_log_fn_t* sink_log;
    free_fn_t* free_fn;
    void* sink_data;
} Sink;

// stderror sink
Sink* stderror_sink_create(void);
sink_log_fn_t stderror_sink;
// end stderror sink

// file sink
Sink* file_sink_create(const char* filename);
sink_log_fn_t file_sink;
// end file sink

#endif // end TMB_SINK_H
