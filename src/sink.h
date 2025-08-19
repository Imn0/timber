#ifndef TMB_SINK_H_
#define TMB_SINK_H_
#include <tmb_internal.h>

typedef void tmb_sink_fn(const char*, int, void*);

typedef struct tmb_sink {
    tmb_sink_fn* sink_fn;
    free_fn_t* free_fn;
    void* sink_data;
} tmb_sink_t;

tmb_sink_t tmb_sink_fd_make(FILE* fd);
#define TMB_SINK_STDOUT() tmb_sink_fd_make(stdout)
#define TMB_SINK_STDERR() tmb_sink_fd_make(stderr)
#define TMB_SINK_FD(fd)   tmb_sink_fd_make(fd)

#endif
