#include <stddef.h>
#include <tmb_lib.h>

typedef struct {
    _hm_header_(char*, int);
} hash_map;

typedef struct {
    _hm_header_(const char*, int);
} c_hash_map;

int main() {
    hash_map hm = { 0 };
    char* p1 = tmb_strdup("a very long string so its easy to see in the debbuger");
    char* p2 = tmb_strdup("a very long string so its easy to see in the debbuger");
    hm_put(&hm, p1, 12);
    int a = hm_get(&hm, p2);
    ASSERT(a == 12);
    ASSERT(hm.key_type == KEY_STR);

    c_hash_map hm2 = { 0 };
    char* p12 = tmb_strdup("a very long string so its easy to see in the debbuger");
    char* p22 = tmb_strdup("a very long string so its easy to see in the debbuger");

    hm_put(&hm2, p12, 12);
    int a2 = hm_get(&hm, p22);
    ASSERT(a2 == 12);
    ASSERT(hm2.key_type == KEY_STR);
}
