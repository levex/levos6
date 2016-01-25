#include <levos/levos.h>

uint8_t inportb(uint16_t portid)
{
    uint8_t ret;
    asm volatile("inb %%dx, %%al":"=a"(ret):"d"(portid));
    return ret;

}

uint16_t inportw(uint16_t p)
{
    uint16_t r;
    asm volatile("inw %%dx, %%ax":"=a"(r):"d"(p));
    return r;
}

void outportb(uint16_t portid, uint8_t value)
{
    asm volatile("outb %%al, %%dx"::
            "d"(portid), "a"(value));
}

extern page_dir_t *current_directory;
void switch_page_dir(page_dir_t *d)
{
    current_directory = d;
    asm volatile (
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "orl $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :: "r"(d->phy_addr)
        : "%eax");
}
