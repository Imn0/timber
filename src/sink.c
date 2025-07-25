#include <sink.h>

// stderror sink
Sink stderror_sink_create(void) {
    return (Sink) { .sink_data = NULL,
                    .sink_log  = stderror_sink,
                    .free_fn   = do_nothing };
}

void stderror_sink(const String* msg, void* data) {
    UNUSED data;
    fprintf(stderr, "%.*s", (int)msg->size, msg->items);
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

Sink file_sink_create(const char* filename) {
    FileSinkData* sink_data = malloc(sizeof(FileSinkData));
    sink_data->fd           = fopen(filename, "a");
    if (sink_data->fd == NULL) { perror("cannot open file"); }
    return (Sink) { .sink_data = sink_data,
                    .sink_log  = file_sink,
                    .free_fn   = file_sink_data_destroy

    };
}

void file_sink(const String* msg, void* data) {
    FileSinkData* sink_data = (FileSinkData*)data;
    fprintf(sink_data->fd, "%.*s", (int)msg->size, msg->items);
}
// end file sink
