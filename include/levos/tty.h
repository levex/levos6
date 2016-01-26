#ifndef __LEVOS_TTY_H
#define __LEVOS_TTY_H

#include <levos/levos.h>

#define TTY_COUNT 4
#define TTY_BUF_SIZE 65536

#define TTY_IN_BUFSIZE 8192
#define TTY_OUT_BUFSIZE 8192

extern int tty_init();

extern void tty_set_default_output(struct device *);

struct tty {
    /* where our input comes from */
    struct device *input;
    /* where are we outputting */
    struct device *output;
    /* input buffer */
    uint8_t *inputbuf;
    /* output buffer */
    uint8_t *outputbuf;
};

#endif /* __LEVOS_TTY_H */
