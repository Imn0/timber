#include <sink.h>

// stderror sink
Sink* stderror_sink_create(void) {
    Sink* sink = malloc(sizeof(Sink));
    sink->sink_data = NULL;
    sink->sink_log = stderror_sink;
    sink->free_fn = do_nothing;

    return sink;
}

void stderror_sink(const cstr msg, void* data) {
    UNUSED data;
    fprintf(stderr, "%s", msg);
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

Sink* file_sink_create(const char* filename) {
    Sink* sink = malloc(sizeof(Sink));
    sink->sink_log = file_sink;
    sink->free_fn = file_sink_data_destroy;
    FileSinkData* sink_data = malloc(sizeof(FileSinkData));
    sink_data->fd = fopen(filename, "a");
    if(sink_data->fd == NULL){
        perror("cannot open file");
    }
    sink->sink_data = sink_data;
    return sink;
}

void file_sink(const cstr msg, void* data) {
    FileSinkData* sink_data = (FileSinkData*)data;
    fprintf(sink_data->fd, "%s", msg);
}
// end file sink
