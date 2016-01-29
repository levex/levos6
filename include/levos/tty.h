#ifndef __LEVOS_TTY_H
#define __LEVOS_TTY_H

#include <levos/levos.h>

#define TTY_COUNT 4
#define TTY_BUF_SIZE 65536

#define TTY_IN_BUFSIZE 8192
#define TTY_OUT_BUFSIZE 8192

extern int tty_init();

extern void tty_set_default_output(struct device *);


extern struct tty *get_tty(int i);

struct tty {
    int tty_id;
    /* where our input comes from */
    struct device *input;
    /* where are we outputting */
    struct device *output;
    /* input buffer */
    uint8_t *inputbuf;
    /* current index of inputbuf */
    int inputidx;
    /* output buffer */
    uint8_t *outputbuf;
    /* current index of outputbuf */
    int outputidx;
    /* internal device representation of the tty */
    struct device *selfdevice;

    int buffered;
} __attribute__((packed));

struct tty_private {
    struct tty *tty;
    struct console **consoles;
    int num_consoles;
    int idx_active_console;
} __attribute__((packed));

extern void tty_set_output(struct tty *tty, struct device *out);
extern int tty_output_write(struct device *, const void *, size_t, size_t);
extern void tty_set_buffered(struct tty *tty, int buf);

#endif /* __LEVOS_TTY_H */
