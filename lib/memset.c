#include <stdint.h>
#include <levos/levos.h>

void memcpy32(uint32_t *dest, uint32_t *src, uint32_t count)
{
    uint32_t *dst32 = (uint32_t *)dest;
    uint32_t *src32 = (uint32_t *)src;

    for (int i = 0; i < count; i++)
        *dst32 ++ = *src32 ++;

}

void memcpy(void *dest, void *src, size_t count)
{
    char *dst8 = (char*)dest;
    char *src8 = (char*)src;

    if (count & 1) {
        dst8[0] = src8[0];
        dst8 += 1;
        src8 += 1;
    }

    count /= 2;
    while (count --) {
        dst8[0] = src8[0];
        dst8[1] = src8[1];

        dst8 += 2;
        src8 += 2;
    }
}

void *memset(void *ptr, char value, size_t num)
{
        uint8_t *p = ptr;
        while(num --)
                *p ++ = value;
        return ptr;
}

void *memset16(void *ptr, uint16_t value, size_t num)
{
        uint16_t *p = ptr;
        while(num --)
                *p ++ = value;
        return ptr;
}
