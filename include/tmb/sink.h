#ifndef TMB_SINK_H_
#define TMB_SINK_H_

#include "tmb.h"
#include <stdio.h>

TMB_API tmb_sink_t tmb_sink_fd_make(FILE* fd);
TMB_API void tmb_sink_deinit(tmb_sink_t* sink);
#define TMB_SINK_STDOUT() tmb_sink_fd_make(stdout)
#define TMB_SINK_STDERR() tmb_sink_fd_make(stderr)
#define TMB_SINK_FD(fd)   tmb_sink_fd_make(fd)

#endif
