#include <levos/levos.h>

size_t strlen(const uint8_t *str)
{
        size_t i = 0;
        while(str[i] != 0) i++;
        return i;
}

size_t strcmp(char *str1, char *str2)
{
    size_t res=0;
    while (!(res = *(unsigned char*)str1 - *(unsigned char*)str2) && *str2)
        ++str1, ++str2;

    return res;
}

size_t strncmp(char *str1, char *str2, size_t n)
{
    while (n--)
        if (*str1++ != *str2++)
            return *(unsigned char *)(str1 - 1) - *(unsigned char *)(str2 - 1);
    return 0;
}

static char *sp = 0;

char *strtok(char *str, const char *delim)
{
    int i = 0;
    int len = strlen(delim);

    if (len == 0)
        return 0;

    if (!str && !sp)
        return 0;

    if (str && !sp)
        sp = str;

    char *p_start = sp;
    while(1) {
        for(i = 0; i < len; i++) {
            if (*p_start == delim[i]) {
                p_start ++;
                break;
            }
        }

        if (i == len) {
            sp = p_start;
            break;
        }
    }

    if (*sp == 0) {
        sp = 0;
        return sp;
    }

    while (*sp != 0) {
        for (i = 0; i < len; i++) {
            if (*sp == delim[i]) {
                *sp = 0;
                break;
            }
        }
        sp ++;
        if (i < len)
            break;
    }

    return p_start;
}
