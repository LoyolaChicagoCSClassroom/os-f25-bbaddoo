#include "page.h"

#define PAGE_SIZE_2MB (2 * 1024 * 1024)

struct ppage physical_page_array[128];
struct ppage *free_physical_pages = 0;

void init_pfa_list(void) {
for (int i = 0; i < 128; i++) {
physical_page_array[i].physical_addr = (void *)(i * PAGE_SIZE_2MB);
physical_page_array[i].next = (i < 127) ? &physical_page_array[i + 1] : 0;
physical_page_array[i].prev = (i > 0) ? &physical_page_array[i - 1] : 0;
 }
 free_physical_pages = &physical_page_array[0];
}

struct ppage *allocate_physical_pages(unsigned int npages) {
    if (npages == 0 || free_physical_pages == 0)
        return 0;

    struct ppage *allocd_list = free_physical_pages;
    struct ppage *last = allocd_list;

    for (unsigned int i = 1; i < npages && last->next != 0; i++) {
        last = last->next;
    }

    free_physical_pages = last->next;
    if (free_physical_pages)
        free_physical_pages->prev = 0;

    last->next = 0;
    allocd_list->prev = 0;

    return allocd_list;


}

void return_physical_pages(struct ppage *ppage_list) {
    if (!ppage_list)
        return;

    struct ppage *tail = ppage_list;
    while (tail->next)
        tail = tail->next;

    tail->next = free_physical_pages;
    if (free_physical_pages)
        free_physical_pages->prev = tail;

    ppage_list->prev = 0;
    free_physical_pages = ppage_list;


}
