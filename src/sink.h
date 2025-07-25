#ifndef TMB_SINK_H
#define TMB_SINK_H

#include <tmb_lib.h>

// stderror sink
Sink stderror_sink_create(void);
sink_log_fn_t stderror_sink;
// end stderror sink

// file sink
Sink file_sink_create(const char* filename);
sink_log_fn_t file_sink;
// end file sink

#endif // end TMB_SINK_H
