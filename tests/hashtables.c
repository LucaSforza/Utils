#include "include.c"

#define N_BUCKETS 1024

void test_sv_hash(void) {
    String_View hello = sv_from_cstr("hello");
    String_View world = sv_from_cstr("world");
    assert(sv_hash(hello,N_BUCKETS) == 532);
    assert(sv_hash(world,N_BUCKETS) == 552);
}

void test_set_get(void) {
    String_View hello = sv_from_cstr("hello");
    String_View world = sv_from_cstr("world");
    String_View smile = sv_from_cstr(":))))");
    HashMap_Sv_Int hasht = init_hashmap_sv_int(N_BUCKETS);
    ht_set(&hasht, hello, 5);
    ht_set(&hasht, world, 8);

    assert(*ht_get(&hasht, hello) == 5);
    assert(*ht_get(&hasht, world) == 8);
    assert(ht_get(&hasht, smile) == NULL);

    ht_free(hasht);
}

int main(void) {
    test_sv_hash();
    test_set_get();
    return 0;
}