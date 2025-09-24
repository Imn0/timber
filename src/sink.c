#include <sink_internal.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

struct sink_fd_data {
    FILE* fd;
};

static void fd_sink(const char* msg, int msg_len, void* data) {
    struct sink_fd_data* sink_data = (struct sink_fd_data*)data;
    fprintf(sink_data->fd, "%.*s", msg_len, msg);
}

tmb_sink_t tmb_sink_fd_make(FILE* fd) {
    struct sink_fd_data* fd_malloced = malloc(sizeof(*fd_malloced));
    fd_malloced->fd                  = fd;
    return (tmb_sink_t) { .sink_data = (void*)fd_malloced,
                          .sink_fn   = fd_sink,
                          .free_fn   = free };
}


static void buffer_sink(const char* msg, int msg_len, void* data) {
    struct sink_buffer_data* buff_data = data;
    int copy_len                       = 0;
    if (msg_len > buff_data->buffer_size)
        copy_len = buff_data->buffer_size;
    else
        copy_len = msg_len;

    for (int i = 0; i < copy_len; i++) { buff_data->items[i] = msg[i]; }
    buff_data->length=copy_len;
}

tmb_sink_t tmb_sink_buffer_make(int buff_size) {
    struct sink_buffer_data* buff_data = malloc(sizeof(*buff_data) +
                                                (size_t)buff_size);
    buff_data->buffer_size             = buff_size;
    return (tmb_sink_t) { .sink_data = (void*)buff_data,
                          .sink_fn   = buffer_sink,
                          .free_fn   = free };
}

TMB_API void tmb_sink_deinit(tmb_sink_t* sink) {
    if (sink->free_fn != NULL) { sink->free_fn(sink->sink_data); }
}
