#include <sink_internal.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

struct sink_fd_data {
    FILE* fd;
};

static void close_and_free_fd(void* data) {
    struct sink_fd_data* sink_data = data;
    (void)fclose(sink_data->fd);
    free(data);
}

TMB_API tmb_sink_t* tmb_sink_file_create(const char* filename) {
    FILE* fd = fopen(filename, "a");
    if (fd == NULL) {}

    tmb_sink_t* sink = tmb_sink_fd_create(fd);
    sink->data_free_fn    = close_and_free_fd;
    return sink;
}
