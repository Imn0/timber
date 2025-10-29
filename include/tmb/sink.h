#ifndef TMB_SINK_H_
#define TMB_SINK_H_

#include "tmb.h"
#include <stdio.h>

TMB_API tmb_sink_t tmb_sink_fd_make(FILE* fd);
#define TMB_SINK_STDOUT() tmb_sink_fd_make(stdout)
#define TMB_SINK_STDERR() tmb_sink_fd_make(stderr)
#define TMB_SINK_FD(fd)   tmb_sink_fd_make(fd)

TMB_API tmb_sink_t tmb_sink_file_make(const char* filename);
#define TMB_SINK_FILE(filename) tmb_sink_file_make(filename)

TMB_API tmb_sink_t tmb_sink_rotating_file_make(const char* filename);
#define TMB_SINK_ROTATING_FILE(filename) tmb_sink_rotating_file_make(filename)

TMB_API tmb_sink_t tmb_sink_graylog_make(const char* graylog_host, int port);
#define TMB_SINK_GRAYLOG(graylog_host, port)                                   \
    tmb_sink_graylog_make(graylog_host, port)

TMB_API void tmb_sink_deinit(tmb_sink_t* sink);
#endif
