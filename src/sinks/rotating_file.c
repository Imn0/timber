#include <sink_internal.h>
#include <tmb/sink.h>
#include <tmb_internal.h>

struct rotating_file_data {
    FILE* fd;
    tmb_mutex_t sink_mtx;
    tmb_string_view_t base_filename;
    int max_num_files;
    int max_file_size_B;
    int current_written_B;
};

#define TMB_PATH_BUFF_SIZE 1024

static void rotate_files(struct rotating_file_data* sink_data) {
    char path_buf[TMB_PATH_BUFF_SIZE];

    const char* base = sink_data->base_filename.items;
    int max_files    = sink_data->max_num_files;

    snprintf(path_buf, sizeof(path_buf), "%s.%d", base, max_files - 1);
    remove(path_buf);
    fclose(sink_data->fd);

    for (int i = max_files - 2; i >= 0; --i) {
        char src[TMB_PATH_BUFF_SIZE];
        if (i == 0)
            snprintf(src, sizeof(src), "%s", base);
        else
            snprintf(src, sizeof(src), "%s.%d", base, i);
        snprintf(path_buf, sizeof(path_buf), "%s.%d", base, i + 1);
        rename(src, path_buf);
    }

    sink_data->fd                = fopen(base, "a");
    sink_data->current_written_B = 0;
}

static void rotating_file_sink(const char* msg, int msg_len, void* data) {
    struct rotating_file_data* sink_data = data;

    tmb_mutex_lock(&sink_data->sink_mtx);

    int new_size = sink_data->current_written_B + msg_len;
    if (new_size > sink_data->max_file_size_B) { rotate_files(sink_data); }

    if (sink_data->fd) {
        fprintf(sink_data->fd, "%.*s", msg_len, msg);
        fflush(sink_data->fd);
        sink_data->current_written_B += msg_len;
    }
    tmb_mutex_unlock(&sink_data->sink_mtx);
}

TMB_API tmb_sink_t* tmb_sink_rotating_file_create(const char* filename,
                                                  int max_files,
                                                  int max_file_size_B) {
    tmb_sink_t* sink                         = malloc(sizeof(*sink));
    struct rotating_file_data* rot_file_data = malloc(sizeof(*rot_file_data));

    tmb_string_builder_t sb = { 0 };

    sb_append_cstr(&sb, filename);
    sb_to_cstr(&sb);
    tmb_string_view_t sv = sv_from_sb(&sb);

    FILE* fd                         = fopen(sv.items, "a");
    rot_file_data->base_filename     = sv;
    rot_file_data->fd                = fd;
    rot_file_data->max_file_size_B   = max_file_size_B;
    rot_file_data->max_num_files     = max_files;
    rot_file_data->current_written_B = 0;

    sink->sink_data = (void*)rot_file_data;
    sink->ref_count = 0;
    sink->sink_fn   = rotating_file_sink;
    sink->data_free_fn   = free;
    return sink;
}
