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
    vsnprintf(sb->items + sb->length, n + 1, fmt, args1);
    va_end(args1);

    sb->length += n;
}

void do_nothing__(void* _data) {
    UNUSED _data;
}

bool load_entire_file(const char* file, tmb_string_builder_t* sb) {
    bool result = true;

    FILE* f = fopen(file, "rb");
    if (f == NULL) {
        result = false;
        goto error;
    }

    int new_length = 0;
    long long size = 0;

    if (fseek(f, 0, SEEK_END) < 0) {
        result = false;
        goto error;
    }

    size = ftell(f);
    if (size < 0) {
        result = false;
        goto error;
    }

    if (fseek(f, 0, SEEK_SET) < 0) {
        result = false;
        goto error;
    }

    new_length = sb->length + (int)size;
    da_reserve(sb, new_length);

    fread(sb->items + sb->length, (size_t)size, 1, f);

    sb->length = new_length;

error:
    if (!result) { printf("AAA"); }
    if (f) { fclose(f); }
    return result;
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
        append_right  = (total_pad + 1) / 2;
        append_left   = total_pad - append_right;
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
        sb_appendn(&left_pad, sb->length, sb->items);
        sb_free(sb);
        sb->items    = left_pad.items;
        sb->capacity = left_pad.capacity;
        sb->length   = left_pad.length;
    }
    if (append_right > 0) {
        for (int i = 0; i < append_right; i++) { sb_append(sb, pad_char); }
    }
}

tmb_string_view_t sv_from_sb(tmb_string_builder_t* sb) {
    tmb_string_view_t sv = { .items = sb->items, .length = sb->length };
    sb->items            = NULL;
    sb->length           = 0;
    sb->capacity         = 0;
    return sv;
}

void tmb_sb_truncate(tmb_string_builder_t* sb,
                     enum tmb_sb_truncate_opt truncate_setting,
                     int max_len) {
    int current_size = sb->length;
    if (max_len >= current_size) { return; }
    int start_idx;
    int append_len;
    switch (truncate_setting) {
    case TRUNCATE_RIGHT:
        start_idx  = 0;
        append_len = max_len;
        break;
    case TRUNCATE_LEFT:
        start_idx  = current_size - max_len;
        append_len = max_len;
        break;
    case TRUNCATE_OFF:
    default:
        return;
    }
    tmb_string_builder_t truncated = { 0 };
    sb_appendn(&truncated, append_len, &sb->items[start_idx]);
    sb_free(sb);
    sb->items    = truncated.items;
    sb->length   = truncated.length;
    sb->capacity = truncated.capacity;
}

void tmb_sb_just_inplace(tmb_string_builder_t* sb,
                         int start_pos,
                         int content_len,
                         enum tmb_sb_just_opt just_setting,
                         int amount,
                         char pad_char) {
    if (amount <= content_len || just_setting == JUST_OFF) { return; }

    int total_pad = amount - content_len;
    int pad_left  = 0;
    int pad_right = 0;

    switch (just_setting) {
    case JUST_LEFT:
        pad_right = total_pad;
        break;
    case JUST_CENTER:
        pad_left  = total_pad / 2;
        pad_right = total_pad - pad_left;
        break;
    case JUST_RIGHT:
        pad_left = total_pad;
        break;
    case JUST_OFF:
    default:
        return;
    }

    da_reserve(sb, start_pos + amount);

    if (pad_left > 0) {
        memmove(sb->items + start_pos + pad_left,
                sb->items + start_pos,
                (size_t)content_len);

        memset(sb->items + start_pos, pad_char, (size_t)pad_left);
    }

    if (pad_right > 0) {
        memset(sb->items + start_pos + pad_left + content_len,
               pad_char,
               (size_t)pad_right);
    }

    sb->length = start_pos + amount;
}

void tmb_sb_truncate_inplace(tmb_string_builder_t* sb,
                             int start_pos,
                             int content_len,
                             enum tmb_sb_truncate_opt truncate_setting,
                             int max_len) {
    if (max_len >= content_len || truncate_setting == TRUNCATE_OFF) { return; }

    switch (truncate_setting) {
    case TRUNCATE_RIGHT:
        sb->length = start_pos + max_len;
        break;

    case TRUNCATE_LEFT:
        memmove(sb->items + start_pos,
                sb->items + start_pos + (content_len - max_len),
                (size_t)max_len);
        sb->length = start_pos + max_len;
        break;

    case TRUNCATE_OFF:
    default:
        break;
    }
}

static u64 hash_djb2(const void* addr_of_key, size_t key_size) {
    const u8* a = addr_of_key;
    u64 hash    = 5381;
    for (size_t i = 0; i < key_size; i++) {
        hash = ((hash << 5) + hash) + a[i];
    }
    return hash;
}

/**
 * @brief Compares two keys with eachother, if key_type is KEY_RAW then value
 * refference by pointers will be compared, if it's KEY_STR then value will be
 * derefferenced and compared using strcmp
 *
 * @param key1
 * @param key2
 * @param key_size
 * @param key_type
 * @return int
 */
static int hm_cmp(const void* key1,
                  const void* key2,
                  size_t key_size,
                  enum tmb_hm_key_type__ key_type) {
    if (key_type == KEY_RAW) { return memcmp(key1, key2, key_size); }
    if (key_type == KEY_STR) { return strcmp(*(char**)key1, *(char**)key2); }
    return 0;
}
static void tmb_hm_grow(void* user_hm,
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
        u32 j = hash;

        while (true) {
            u8* new_bucket_base = (u8*)(new_buckets + (size_t)j * bucket_size);
            bool occupied       = *(bool*)(new_bucket_base + occupied_offset);

            if (!occupied) {
                memcpy(new_bucket_base, old_bucket_base, bucket_size);
                *(bool*)(new_bucket_base + occupied_offset +
                         sizeof(bool)) = false;
                break;
            }
            j = (j + 1) % (u32)new_capacity;
        }
    }

    free(hm->buckets);
    hm->buckets  = (void*)new_buckets;
    hm->capacity = new_capacity;
}

static void tmb_hm_shrink(void* user_hm,
                          size_t bucket_size,
                          size_t buckets_offset,
                          size_t key_size,
                          size_t key_offset,
                          size_t occupied_offset) {
    tmb_hash_map_internal* hm = user_hm;
    u8* buckets               = *(u8**)(void*)((u8*)user_hm + buckets_offset);

    int new_capacity = hm->capacity / 2;

    if (new_capacity < TMB_HM_DEFAULT_START_SIZE) {
        new_capacity = TMB_HM_DEFAULT_START_SIZE;
    }

    if (new_capacity == hm->capacity) { return; }

    u8* new_buckets = calloc((size_t)new_capacity, bucket_size);

    for (int i = 0; i < hm->capacity; i++) {
        u8* old_bucket_base  = (u8*)(buckets + (size_t)i * bucket_size);
        bool is_old_occupied = *(bool*)(old_bucket_base + occupied_offset);

        if (!is_old_occupied) { continue; }

        u8* addr_of_old_key = (old_bucket_base + key_offset);

        u32 hash = (u32)hash_djb2(addr_of_old_key, key_size) %
                   (u32)new_capacity;
        u32 j = hash;

        while (true) {
            u8* new_bucket_base = (u8*)(new_buckets + (size_t)j * bucket_size);
            bool occupied       = *(bool*)(new_bucket_base + occupied_offset);

            if (!occupied) {
                memcpy(new_bucket_base, old_bucket_base, bucket_size);
                // clear tombstone flag in new table
                *(bool*)(new_bucket_base + occupied_offset +
                         sizeof(bool)) = false;
                break;
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
                        const void* key,
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
    void* key_addr        = (u8*)hm->tmp + key_offset;
    void* val_addr        = (u8*)hm->tmp + value_offset;
    bool* is_occupied_ptr = (bool*)((u8*)hm->tmp + occupied_offset);

    if (!*is_occupied_ptr) {
        if (hm->key_type == KEY_STR) {
            *(char**)key_addr = tmb_strdup(*(char**)key);
        } else {
            memcpy(key_addr, key, key_size);
        }
        *is_occupied_ptr = true;
        hm->occupied++;
    }
    memcpy(val_addr, value, value_size);
}

void tmb_hm_del_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        const void* addr_of_new_key,
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

    if (hm->key_type == KEY_STR) {
        void* key_addr = (u8*)hm->tmp + key_offset;
        char* str_key  = *(char**)key_addr;
        if (str_key != NULL) {
            free(str_key);
            *(char**)key_addr = NULL;
        }
    }

    hm->tmp->occupied  = false;
    hm->tmp->tombstone = true;
    hm->occupied--;

    float occupancy = (float)hm->occupied / (float)hm->capacity;
    if (hm->capacity > TMB_HM_DEFAULT_START_SIZE && occupancy < 0.25f) {
        tmb_hm_shrink(user_hm,
                      bucket_size,
                      buckets_offset,
                      key_size,
                      key_offset,
                      occupied_offset);
    }
}

void tmb_hm_get_wrapper(void* user_hm,
                        size_t bucket_size,
                        size_t buckets_offset,
                        const void* addr_of_new_key,
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

    u32 i                   = hash;
    u32 first_tombstone_idx = (u32)-1;
    bool has_tombstone      = false;

    for (u32 probes = 0; probes < (u32)hm->capacity; probes++) {
        u8* bucket_base          = (u8*)(buckets + (size_t)i * bucket_size);
        u8* addr_of_existing_key = (bucket_base + key_offset);
        bool occupied            = *(bool*)(bucket_base + occupied_offset);
        bool tombstone = *(bool*)(bucket_base + occupied_offset + sizeof(bool));

        // If we find the key, use this slot
        if (occupied && hm_cmp(addr_of_new_key,
                               addr_of_existing_key,
                               key_size,
                               hm->key_type) == 0) {
            hm->tmp = (void*)(buckets + ((size_t)i * bucket_size));
            return;
        }

        if (!has_tombstone && tombstone && !occupied) {
            first_tombstone_idx = i;
            has_tombstone       = true;
        }

        if (!occupied && !tombstone) {
            if (has_tombstone) {
                hm->tmp = (void*)(buckets +
                                  ((size_t)first_tombstone_idx * bucket_size));
            } else {
                hm->tmp = (void*)(buckets + ((size_t)i * bucket_size));
            }
            return;
        }

        i = (i + 1) % (u32)hm->capacity;
    }

    if (has_tombstone) {
        hm->tmp = (void*)(buckets +
                          ((size_t)first_tombstone_idx * bucket_size));
    } else {
        hm->tmp = (void*)(buckets + ((size_t)hash * bucket_size));
    }
}

// tmb_fabsf and tmb_fabs impl from MUSL
static float tmb_fabsf(float x) {
    union {
        float f;
        uint32_t i;
    } u = { x };
    u.i &= 0x7fffffff;
    return u.f;
}

// static double tmb_fabs(double x) {
//     union {
//         double f;
//         uint64_t i;
//     } u = { x };
//     u.i &= -1ULL / 2;
//     return u.f;
// }

f32 tmb_timestamp_diff(tmb_timestamp_t ts1, tmb_timestamp_t ts2) {
    f32 d = 0;
    d += (f32)(ts1.nsec - ts2.nsec);
    d /= NSEC_IN_SEC;
    d += (f32)(ts1.sec - ts2.sec);

    return d;
}

bool tmb_cmp_flt_impl(f32 a, f32 b, struct tmb_cmp_flt_opt opt) {
    return tmb_fabsf(a - b) < opt.eps;
}

char* tmb_strdup(const char* const s) {
    if (s == NULL) { return NULL; }
    size_t str_size_w_null_terminator = strlen(s) + 1;
    char* b                           = malloc(str_size_w_null_terminator);
    if (!b) return NULL;
    memcpy(b, s, str_size_w_null_terminator);
    return b;
}

bool tmb_sv_cmp(const tmb_string_view_t* sv1, const tmb_string_view_t* sv2) {
    if (sv1->length != sv2->length) return false;

    for (int i = 0; i < sv1->length; i++) {
        if (sv1->items[i] != sv2->items[i]) return false;
    }
    return true;
}

char* tmb_sv_to_ctst_copy(const tmb_string_view_t* sv) {
    char* s = malloc(sizeof(*s) * (size_t)sv->length + 1);

    memcpy(s, sv->items, (unsigned long)sv->length);
    s[sv->length] = '\0';
    return s;
}

bool tmb_is_substring(tmb_string_view_t haystack, const char* needle) {
    size_t needle_len = strlen(needle);
    size_t needle_idx = 0;
    for (int i = 0; i < haystack.length; i++) {
        if (needle[needle_idx] == haystack.items[i]) {
            needle_idx++;
            if (needle_idx == needle_len) { return true; }
        } else {
            needle_idx = 0;
        }
    }
    return false;
}

void tmb_mutex_lock(tmb_mutex_t* mtx) {
#if defined(TMB_THREADING_C11)
    mtx_lock(&mtx->mtx);
#elif defined(TMB_THREADING_PTHREADS)

#elif defined(TMB_THREADING_WINDOWS)

#elif defined(TMB_THREADING_NONE)

#endif
}
void tmb_mutex_unlock(tmb_mutex_t* mtx) {
#if defined(TMB_THREADING_C11)
    mtx_unlock(&mtx->mtx);
#elif defined(TMB_THREADING_PTHREADS)

#elif defined(TMB_THREADING_WINDOWS)

#elif defined(TMB_THREADING_NONE)

#endif
}

void tmb_mutex_init(tmb_mutex_t* mtx) {
#if defined(TMB_THREADING_C11)
    mtx_init(&mtx->mtx, mtx_plain);
#elif defined(TMB_THREADING_PTHREADS)

#elif defined(TMB_THREADING_WINDOWS)

#elif defined(TMB_THREADING_NONE)

#endif
}
