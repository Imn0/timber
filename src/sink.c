#include <sink.h>

// stderror sink
Sink* stderror_sink_create(void) {
    Sink* sink = malloc(sizeof(Sink));
    sink->sink_data = NULL;
    sink->sink_log = stderror_sink;
    sink->free_fn = do_nothing;

    return sink;
}

void stderror_sink(const char* msg, void* data) {
    UNUSED data;
    fprintf(stderr, "%s", msg);
}
// end stderror sink
