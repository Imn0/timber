#include <tmb_lib.h>

#include <stdio.h>

void sb_appendf__(tmb_string_builder_t* sb, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    sb_appendv__(sb, fmt, args);
    va_end(args);
}

void sb_appendv__(tmb_string_builder_t* sb, const char* fmt, va_list args) {

    va_list args1;
    va_copy(args1, args);

    int n = vsnprintf(NULL, 0, fmt, args1);
    va_end(args1);

    da_reserve(sb, sb->size + n + 1);

    va_copy(args1, args);
    vsnprintf(sb->items + sb->size, (unsigned long)n + 1, fmt, args1);
    va_end(args1);

    sb->size += n;
}

void do_nothing__(void* _data) {
    UNUSED _data;
}

char* load_entire_file(const char* file) {
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        perror("cannot open file");
        goto error_return_only;
    }

    if (fseek(f, 0L, SEEK_END) != 0) {
        goto error_close;
        perror("error reading file size");
    }
    long buffsize = ftell(f);
    if (buffsize == -1) {
        goto error_close;
        perror("error reading file size");
    }
    char* file_conents = malloc(sizeof(char) * ((size_t)buffsize + 1));
    if (fseek(f, 0L, SEEK_SET) != 0) {
        goto error_free;
        perror("error reading file size");
    }

    fread(file_conents, (size_t)buffsize, 1, f);
    file_conents[buffsize] = '\0';
    fclose(f);

    return file_conents;

error_free:
    free(file_conents);
error_close:
    fclose(f);
error_return_only:
    return NULL;
}

void tmb_sb_just(tmb_string_builder_t* sb,
                 enum tmb_sb_just_opt just_setting,
                 int amount,
                 char pad_char) {
    int current_size = sb->size;
    if (amount <= current_size) { return; }
    int append_left  = 0;
    int append_right = 0;
    switch (just_setting) {
    case JUST_LEFT:
        append_right = amount - current_size;
        break;
    case JUST_CENTER:
        int total_pad = amount - current_size;
        append_left   = (total_pad + 1) / 2;
        append_right  = total_pad - append_left;
        break;
    case JUST_RIGHT:
        append_left = amount - current_size;
        break;
    case JUST_OFF:
    default:
        return;
    }

    if (append_left > 0) {
        tmb_string_builder_t left_pad = { 0 };
        for (int i = 0; i < append_left; i++) {
            sb_append(&left_pad, pad_char);
        }
        sb_appendn(&left_pad, sb->items, sb->size);
        sb_free(sb);
        sb->items    = left_pad.items;
        sb->capacity = left_pad.capacity;
        sb->size     = left_pad.size;
    }
    if (append_right > 0) {
        for (int i = 0; i < append_right; i++) { sb_append(sb, pad_char); }
    }
}

void tmb_sb_truncate(tmb_string_builder_t* sb,
                     enum tmb_sb_truncate_opt truncate_setting,
                     int max_len) {
    int current_size = sb->size;
    if (max_len >= current_size) { return; }
    int start_idx;
    int append_len;
    switch (truncate_setting) {
    case TRUNCATE_LEFT:
        start_idx  = 0;
        append_len = max_len;
        break;
    case TRUNCATE_RIGHT:
        start_idx  = current_size - max_len;
        append_len = max_len;
        break;
    case TRUNCATE_OFF:
    default:
        return;
    }
    tmb_string_builder_t truncated = { 0 };
    sb_appendn(&truncated, &sb->items[start_idx], append_len);
    sb_free(sb);
    sb->items    = truncated.items;
    sb->size     = truncated.size;
    sb->capacity = truncated.capacity;
}

int hm_cmp(void* key1,
           void* key2,
           size_t key_size,
           enum tmb_hm_key_type__ key_type) {
    if (key_type == KEY_RAW) { return memcmp(key1, key2, key_size); }
    if (key_type == KEY_STR) { return strcmp(key1, key2); }
    return 0;
}

void tmb_hm_get_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        void* key,
                        size_t key_size,
                        size_t key_offset,
                        size_t occupied_offset) {
    tmb_hash_map_internal* hm = user_hm;
    u8* buckets               = *(u8**)(void*)((u8*)hm + buckets_offset);
    for (int i = 0; i < hm->occupied; i++) {
        u8* bucket_base = (u8*)(buckets + (size_t)i * bucket_size);
        u8* key2        = bucket_base + key_offset;
        bool occupied   = *(bool*)(bucket_base + occupied_offset);
        if (occupied && hm_cmp(key, key2, key_size, hm->key_type) == 0) {
            hm->tmp = (void*)(buckets + ((size_t)i * bucket_size));
            return;
        }
    }
    hm->tmp = (void*)(buckets + ((size_t)hm->occupied * bucket_size));
}
