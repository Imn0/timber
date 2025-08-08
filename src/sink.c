#include <tmb_lib.h>

// stderror sink
void stderror_sink(const String* msg, void* data) {
    UNUSED data;
    fprintf(stderr, "%.*s", (int)msg->size, msg->items);
}

Sink tmb_sink_stderror_make(void) {
    return (Sink) { .sink_data = NULL,
                    .sink_log  = stderror_sink,
                    .free_fn   = do_nothing };
}
// end stderror sink

// file sink
typedef struct {
    FILE* fd;
} FileSinkData;

void file_sink_data_destroy(void* data) {
    FileSinkData* sink_data = (FileSinkData*)data;
    fclose(sink_data->fd);
    free(data);
}

void file_sink(const String* msg, void* data) {
    FileSinkData* sink_data = (FileSinkData*)data;
    fprintf(sink_data->fd, "%.*s", (int)msg->size, msg->items);
}

Sink tmb_sink_file_make(const char* filename) {
    FileSinkData* sink_data = malloc(sizeof(*sink_data));
    sink_data->fd           = fopen(filename, "a");
    if (sink_data->fd == NULL) { perror("cannot open file"); }
    return (Sink) { .sink_data = sink_data,
                    .sink_log  = file_sink,
                    .free_fn   = file_sink_data_destroy

    };
}
// end file sink

// rotating file sink
typedef struct {
    const char* base_filename;
    size_t current_log;
    size_t max_size;
    size_t written;
    FILE* fd;

} RotatingFileSinkData;

void rotating_file_sink(const String* msg, void* data) {
    RotatingFileSinkData* sink_data = (RotatingFileSinkData*)data;
    if (msg->size + sink_data->written > sink_data->max_size) {
        // rotate
        fclose(sink_data->fd);
        sink_data->current_log++;
        StringBuilder sb = { 0 };
        sb_append_cstr(&sb, sink_data->base_filename);
        sb_appendf(&sb, "%lu", sink_data->current_log);
        sb_to_cstr(&sb);
        sink_data->fd      = fopen(sb.items, "a");
        sink_data->written = 0;
    }
    fprintf(sink_data->fd, "%.*s", (int)msg->size, msg->items);
    sink_data->written += msg->size;
}

Sink tmb_sink_rotating_file_make(const char* filename, size_t max_size_kb) {
    RotatingFileSinkData* data = malloc(sizeof(*data));

    data->base_filename        = filename;
    data->written              = 0;
    data->max_size             = max_size_kb;
    data->fd                   = fopen(filename, "a");
    data->current_log          = 0;

    return (Sink) { .sink_data = data,
                    .sink_log  = rotating_file_sink,
                    .free_fn   = free };
}

// end rotating file sink
