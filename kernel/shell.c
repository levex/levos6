#include <levos/levos.h>
#include <stdarg.h>

extern void vprintk(char *, ...);
static void _printk(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    vprintk(fmt, ap);

    va_end(ap);
}

void show_prompt()
{
    _printk("LevOS$ ");
}

static struct tty *shell_tty;

extern char keyboard_get_key(void);
extern void __printk_emit(char);
char *grab_input()
{
    static char buffer[512];
    int i = 0;

    memset(buffer, 0, 512);

    while(1) {
        char cs[] = { 0 };
        tty_input_read(shell_tty->selfdevice, cs, 1, 0);
        if (!cs[0]) {
            schedule_noirq();
            continue;
        }
        if (cs[0] == '\n')
            break;
        buffer[i++] = cs[0];
        __printk_emit(cs[0]);
    }
    __printk_emit('\n');
    return buffer;
}

void cmd_ps()
{
    _printk("NAME [PID] extra\n");
    for (int i = 0; i < THREAD_QUEUE_SIZE; i++) {
        struct thread *p = thread_queue[i];

        if (!p)
            continue;

        _printk("%s [%d] %s\n", p->owner->comm, p->owner->pid,
                current == p ? "[current]" : "");
    }
}

void cmd_lsdev()
{
    _printk("NAME TYPE\n");
    for (int i = 0; i < get_num_of_devices(); i++) {
        struct device *d = device_get(i);

        if (!d) {
            i --;
            continue;
        }

        _printk("%s %s\n", d->name,
                d->type == DEV_TYPE_TTY ? "[tty]"
                : d->type == DEV_TYPE_PSEUDO ? "[pseudo]"
                : d->type == DEV_TYPE_ISA ? "[isa]"
                : d->type == DEV_TYPE_PCI ? "[pci]"
                : d->type == DEV_TYPE_BLK ? "[blk]"
                : d->type == DEV_TYPE_TTY_INPUT ? "[intty]"
                : "[unktype]");
    }
}

void cmd_ls(char *arg)
{
    struct file *file = vfs_open(arg);

    if (!file->isdir) {
        _printk("%s", errno_to_string(ENOTDIR));
        return;
    }

    return;
}

void cmd_stat(char *arg)
{
    struct stat st;
    int ret;

    ret = call_syscall(18, (uintptr_t) arg, (uintptr_t) &st, 0);
    if (ret < 0) {
        _printk("Error while stat: %d\n", ret);
        return;
    }
    _printk("File: %s\n", arg);
    _printk("Size: %d bytes\n", st.st_size);
}

void cmd_cat(char *arg)
{
    int fd;
    char *buf;
    struct stat st;
    int ret;

    ret = call_syscall(18, (uintptr_t) arg, (uintptr_t) &st, 0);
    if (ret < 0) {
        _printk("Error while stat: %d\n", ret);
        return;
    }
    buf = kmalloc(st.st_size);
    fd = call_syscall(5, (uintptr_t) arg, 0, 0);
    if (fd <= 0) {
        _printk("open failed\n");
        return;
    }
    ret = call_syscall(3, fd, (uintptr_t) buf, st.st_size);
    call_syscall(4, 1, (uintptr_t) buf, st.st_size);
}

void cmd_exit()
{
    call_syscall(1, 0, 0, 0);
}

static int __i_ = 1000;
static void __cmd_testmt_1() { while (__i_ -= 100 > 0) _printk("%d\n", __i_);
        call_syscall(1, 0, 0, 0);}
static void __cmd_testmt_2() { while (__i_ ++ < 1000) { _printk("%d\n", __i_);
        if (!(__i_ % 100)) schedule_noirq(); }
        call_syscall(1, 0, 0, 0);}

void cmd_testmt()
{
    static struct process *p1;
    static struct process *p2;

    p1 = sched_mk_process("p1", (uintptr_t) __cmd_testmt_1);
    p2 = sched_mk_process("p2", (uintptr_t) __cmd_testmt_2);

    sched_add_process(p1);
    sched_add_process(p2);
}

extern int elf_load(struct file *);
void cmd_elf(char *arg)
{
    int fd, ret;
    struct file *fp;
    int isElf;
    elf_header_t header;

    fd = call_syscall(5, (uintptr_t) arg, 0, 0);
    if (fd <= 0) {
        _printk("open failed: %d\n", fd);
        return;
    }

    fp = current->owner->file_table[fd];
    isElf = elf_probe(fp);
    _printk("file(%d) is %sELF\n", fd, isElf ? "" : "not ");
    if (!isElf) {
        call_syscall(6, fd, 0, 0);
        return;
    }

    fp->fops->read(fp, &header, sizeof(elf_header_t));

    _printk("ELF Type: %s\n", elf_type_as_string(header.e_type));
    _printk("Entry Point: 0x%x\n", header.e_entry);

    ret = elf_load(fp);
    _printk("ELF RET: %s\n", errno_to_string(ret));
}

void parse_input(char *cmd)
{
    if(strcmp("ps", cmd) == 0) {
        cmd_ps();
    } else if (strcmp("lsdev", cmd) == 0) {
        cmd_lsdev();
    } else if (strncmp("ls ", cmd, 3) == 0) {
        cmd_ls(cmd + 3);
    } else if (strcmp("exit", cmd) == 0) {
        cmd_exit();
    } else if (strcmp("testmt", cmd) == 0) {
        cmd_testmt();
    } else if (strncmp("cat ", cmd, 4) == 0) {
        cmd_cat(cmd + 4);
    } else if (strncmp("stat ", cmd, 5) == 0) {
        cmd_stat(cmd + 5);
    } else if (strncmp("elf ", cmd, 4) == 0) {
        cmd_elf(cmd + 4);
    }
}

void kernel_shell_start()
{
    printk("Starting LevOS debug shell...\n");
    shell_tty = get_tty(0);
    while (1) {
        show_prompt();
        char *in = grab_input();
        parse_input(in);
    }
}
