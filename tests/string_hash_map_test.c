#include <stddef.h>
#include <tmb_lib.h>

typedef struct {
    _hm_header_(char*, int);
} hash_map;

typedef struct {
    _hm_header_(const char*, int);
} c_hash_map;

int main() {
    {
        hash_map hm = { 0 };
        char* p1    = tmb_strdup(
                "a very long string so its easy to see in the debbuger");
        char* p2 = tmb_strdup(
                "a very long string so its easy to see in the debbuger");
        hm_put(&hm, p1, 12);
        int a = hm_get(&hm, p2);
        ASSERT(a == 12);
        ASSERT(hm.key_type == KEY_STR);
    }
    {
        hash_map hm = { 0 };
        char* p1    = tmb_strdup(
                "a very long string so its easy to see in the debbuger");
        char* p2 = tmb_strdup(
                "a very long string so its easy to see in the debbuger");
        hm_put(&hm, p1, 12);
        ASSERT(hm_has(&hm,
                      "a very long string so its easy to see in the debbuger"));
        hm_put(&hm, p2, 123);

        int a = hm_get(&hm,
                       "a very long string so its easy to see in the debbuger");
        ASSERT(a == 123);
        ASSERT(hm.key_type == KEY_STR);
    }
    {
        hash_map hm = { 0 };
        char* b_s   = tmb_strdup("b");
        hm_put(&hm, b_s, 12);
        ASSERT(hm_has(&hm, "b"));
        hm_put(&hm, "a", 123);

        int a = hm_get(&hm, "a");
        int b = hm_get(&hm, "b");
        ASSERT(a == 123);
        ASSERT(b == 12);
        ASSERT(hm.key_type == KEY_STR);
    }
}
