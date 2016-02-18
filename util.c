#include <stdint.h>
#include <stddef.h>

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *target = dst, *from = src;
    for(; n > 0; n --) {
        *target = *from;
        target ++, from ++;
    }
    return dst;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *target = s;
    for(; n > 0; n --) {
        *target = c;
        target ++;
    }
    return s;
}
