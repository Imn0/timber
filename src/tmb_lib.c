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

    da_reserve(sb, sb->length + n + 1);

    va_copy(args1, args);
    vsnprintf(sb->items + sb->length, (unsigned long)n + 1, fmt, args1);
    va_end(args1);

    sb->length += n;
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

    size_t read = fread(file_conents, (size_t)buffsize, 1, f);
    if (read != (size_t)buffsize) {
        fprintf(stderr,
                "error reading file, read %lu bytes, expected %lu",
                read,
                buffsize);
    }
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
    int current_size = sb->length;
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
        sb_appendn(&left_pad, sb->items, sb->length);
        sb_free(sb);
        sb->items    = left_pad.items;
        sb->capacity = left_pad.capacity;
        sb->length   = left_pad.length;
    }
    if (append_right > 0) {
        for (int i = 0; i < append_right; i++) { sb_append(sb, pad_char); }
    }
}

void tmb_sb_truncate(tmb_string_builder_t* sb,
                     enum tmb_sb_truncate_opt truncate_setting,
                     int max_len) {
    int current_size = sb->length;
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
    sb->length   = truncated.length;
    sb->capacity = truncated.capacity;
}

u64 hash_djb2(void* addr_of_key, size_t key_size) {
    u8* a    = addr_of_key;
    u64 hash = 5381;
    for (size_t i = 0; i < key_size; i++) {
        hash = ((hash << 5) + hash) + a[i];
    }
    return hash;
}

/**
 * @brief Compares two keys with eachother, if key_type is KEY_RAW then value refference by pointers will be compared,
 * if it's KEY_STR then value will be derefferenced and compared using strcmp
 * 
 * @param key1 
 * @param key2 
 * @param key_size 
 * @param key_type 
 * @return int 
 */
int hm_cmp(void* key1,
           void* key2,
           size_t key_size,
           enum tmb_hm_key_type__ key_type) {
    if (key_type == KEY_RAW) { return memcmp(key1, key2, key_size); }
    if (key_type == KEY_STR) { return strcmp(*(char**)key1, *(char**)key2); }
    return 0;
}

void tmb_hm_grow(void* user_hm,
                 size_t bucket_size,
                 size_t buckets_offset,
                 size_t key_size,
                 size_t key_offset,
                 size_t occupied_offset) {
    tmb_hash_map_internal* hm = user_hm;
    u8* buckets               = *(u8**)(void*)((u8*)user_hm + buckets_offset);

    int new_capacity = hm->capacity * 2;
    u8* new_buckets  = calloc((size_t)new_capacity, bucket_size);

    for (int i = 0; i < hm->capacity; i++) {
        u8* old_bucket_base  = (u8*)(buckets + (size_t)i * bucket_size);
        bool is_old_occupied = *(bool*)(old_bucket_base + occupied_offset);
        if (!is_old_occupied) { continue; }
        u8* addr_of_old_key = (old_bucket_base + key_offset);

        u32 hash = (u32)hash_djb2(addr_of_old_key, key_size) %
                   (u32)new_capacity;
        u32 j      = hash;
        bool found = false;
        while (!found) {
            u8* new_bucket_base = (u8*)(new_buckets + (size_t)j * bucket_size);
            u8* addr_of_new_key = (new_bucket_base + key_offset);
            bool occupied       = *(bool*)(new_bucket_base + occupied_offset);

            if (!occupied || hm_cmp(addr_of_old_key,
                                    addr_of_new_key,
                                    key_size,
                                    hm->key_type) == 0) {
                hm->tmp = (void*)(buckets + ((size_t)j * bucket_size));
                memcpy(new_buckets + (size_t)j * bucket_size,
                       old_bucket_base,
                       bucket_size);
                found = true;
            }
            j = (j + 1) % (u32)new_capacity;
        }
    }
    free(hm->buckets);
    hm->buckets  = (void*)new_buckets;
    hm->capacity = new_capacity;
}

void tmb_hm_set_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        void* key,
                        size_t key_size,
                        size_t key_offset,
                        void* value,
                        size_t value_size,
                        size_t value_offset,
                        size_t occupied_offset) {
    tmb_hash_map_internal* hm = user_hm;
    float occupancy           = (float)hm->occupied / (float)hm->capacity;

    if (hm->capacity > 2 && occupancy > 0.8f) {
        tmb_hm_grow(user_hm,
                    bucket_size,
                    buckets_offset,
                    key_size,
                    key_offset,
                    occupied_offset);
    }

    tmb_hm_get_wrapper(user_hm,
                       bucket_size,
                       buckets_offset,
                       key,
                       key_size,
                       key_offset,
                       occupied_offset);
    void* key_addr      = (u8*)hm->tmp + key_offset;
    void* val_addr      = (u8*)hm->tmp + value_offset;
    bool* occupied_addr = (bool*)((u8*)hm->tmp + occupied_offset);

    if (!*occupied_addr) {
        if (hm->key_type == KEY_STR) {
            *(char**)key_addr = strdup(*(char**)key);
        } else {
            memcpy(key_addr, key, key_size);
        }
        *occupied_addr = true;
        hm->occupied++;
    }
    memcpy(val_addr, value, value_size);
}

void tmb_hm_del_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        void* addr_of_new_key,
                        size_t key_size,
                        size_t key_offset,
                        size_t occupied_offset) {
    tmb_hash_map_internal* hm = user_hm;

    tmb_hm_get_wrapper(user_hm,
                       bucket_size,
                       buckets_offset,
                       addr_of_new_key,
                       key_size,
                       key_offset,
                       occupied_offset);
    if (hm->tmp->occupied != true) { return; }
    if (hm->key_type == KEY_STR) { free(hm->tmp->key); }
    hm->tmp->occupied = false;
}

void tmb_hm_get_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        void* addr_of_new_key,
                        size_t key_size,
                        size_t key_offset,
                        size_t occupied_offset) {
    tmb_hash_map_internal* hm = user_hm;
    u8* buckets               = *(void**)(void*)((u8*)user_hm + buckets_offset);
    hm->tmp                   = NULL;
    u32 hash;
    if (hm->key_type == KEY_STR) {
        hash = hash_djb2(*(u8**)addr_of_new_key, key_size) % (u32)hm->capacity;
    } else {
        hash = hash_djb2(addr_of_new_key, key_size) % (u32)hm->capacity;
    }

    u32 i      = hash;
    bool found = false;
    while (!found) {
        u8* bucket_base          = (u8*)(buckets + (size_t)i * bucket_size);
        u8* addr_of_existing_key = (bucket_base + key_offset);
        bool occupied            = *(bool*)(bucket_base + occupied_offset);
        if (!occupied || hm_cmp(addr_of_new_key,
                                addr_of_existing_key,
                                key_size,
                                hm->key_type) == 0) {
            hm->tmp = (void*)(buckets + ((size_t)i * bucket_size));
            return;
        }
        i = (i + 1) % (u32)hm->capacity;
    }
}
