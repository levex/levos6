#ifndef __PAGE_H
#define __PAGE_H

#include <levos/levos.h>

typedef struct page {
    unsigned int present  :1;
    unsigned int rw       :1;
    unsigned int user     :1;
    unsigned int accessed :1;
    unsigned int dirty    :1;
    unsigned int unused   :7;
    unsigned int frame    :20;
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    uintptr_t phy_tables[1024];
    page_table_t *tables[1024];
    uintptr_t phy_addr;
} page_dir_t;

#endif
