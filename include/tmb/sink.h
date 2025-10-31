#ifndef TMB_SINK_H_
#define TMB_SINK_H_

#include "tmb.h"
#include <stdio.h>

TMB_API tmb_sink_t* tmb_sink_fd_create(FILE* fd);
#define TMB_SINK_STDOUT() tmb_sink_fd_create(stdout)
#define TMB_SINK_STDERR() tmb_sink_fd_create(stderr)
#define TMB_SINK_FD(fd)   tmb_sink_fd_create(fd)

TMB_API tmb_sink_t* tmb_sink_file_create(const char* filename);
#define TMB_SINK_FILE(filename) tmb_sink_file_create(filename)

TMB_API tmb_sink_t* tmb_sink_graylog_create(const char* graylog_host, int port);
#define TMB_SINK_GRAYLOG(graylog_host, port)                                   \
    tmb_sink_graylog_create(graylog_host, port)

TMB_API void tmb_sink_destroy(tmb_sink_t* sink);
#endif
