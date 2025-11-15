#include <sink_internal.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

struct sink_fd_data {
    FILE* fd;
};

static void fd_sink(const char* msg, int msg_len, void* data) {
    struct sink_fd_data* sink_data = (struct sink_fd_data*)data;
    if (sink_data->fd) {
        fprintf(sink_data->fd, "%.*s", msg_len, msg);
        fflush(sink_data->fd);
    }
}

tmb_sink_t* tmb_sink_fd_create(FILE* fd) {
    tmb_sink_t* sink                 = malloc(sizeof(*sink));
    struct sink_fd_data* fd_malloced = malloc(sizeof(*fd_malloced));
    fd_malloced->fd                  = fd;

    sink->sink_data    = (void*)fd_malloced;
    sink->sink_fn      = fd_sink;
    sink->data_free_fn = free;
    sink->ref_count    = 0;
    return sink;
}
