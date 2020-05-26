#include <moonos/arch/tty.h>
#include <moonos/interupt.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <moonos/memory/balloc.h>
#include <moonos/memory/buddy.h>
#include <moonos/memory/paging.h>
#include <moonos/time.h>
#include <multiboot/multiboot.h>
#include <string.h>

static void test_mapping(void) {
    const size_t count = balloc_free_ranges();
    for (size_t i = 0; i != count; i++) {
        struct balloc_range range;
        balloc_get_free_range(i, &range);
        kprintf("Fill 0x%llx - 0x%llx\n", (uint64_t)range.begin,
                (uint64_t)range.end);

        memset(va(range.begin), 42, range.end - range.begin);
    }
}

static size_t allocate_all(int order, list_head_t* list) {
    size_t count = 0;
    while (1) {
        page_t* page = __buddy_alloc(order);
        if (!page) {
            break;
        }

        list_add_tail(&page->ll, list);
        count++;
    }
    return count;
}

static void write_all(int order, list_head_t* list) {
    list_head_t* head = list;

    for (list_head_t* ptr = head->next; ptr != head; ptr = ptr->next) {
        page_t* page = (page_t*)ptr;
        const uintptr_t phys = page_addr(page);

        memset(va(phys), 3148, PAGE_SIZE << order);
    }
}

static void release_all(int order, list_head_t* list) {
    list_head_t* head = list;
    for (list_head_t* ptr = head->next; ptr != head;) {
        page_t* page = (page_t*)ptr;

        ptr = ptr->next;
        __buddy_free(page, order);
    }
    list_init(list);
}

static void buddy_test(void) {
    list_head_t list;
    list_init(&list);

    for (int i = 0; i <= MAX_ORDER; i++) {
        const size_t count = allocate_all(i, &list);

        write_all(i, &list);
        release_all(i, &list);
        if (!count) {
            break;
        }

        kprintf("Allocate %llu blocks of %llu bytes\n", (uint64_t)count,
                (uint64_t)(PAGE_SIZE << i));
    }
}

void main(uintptr_t mb_info_phys) {
    terminal_initialize();

    const struct multiboot_info* info =
            (const struct multiboot_info*)va(mb_info_phys);

    ints_setup();
    balloc_setup(info);
    paging_setup();
    buddy_setup();
    time_setup();
    local_int_enable();

    // test_mapping();
    buddy_test();

    // while (1)
    //     ;
}