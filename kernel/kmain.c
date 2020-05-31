#include <moonos/arch/tty.h>
#include <moonos/cpu/acpi.h>
#include <moonos/cpu/cpuid.h>
#include <moonos/interupt.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <moonos/memory/balloc.h>
#include <moonos/memory/buddy.h>
#include <moonos/memory/paging.h>
#include <moonos/pci/pci.h>
#include <moonos/thread/thread.h>
#include <multiboot/multiboot.h>
#include <string.h>

#if 0
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
#endif

static int threadx(void* arg) {
    const int id = (intptr_t)arg;
    int count = 5;
    while (count) {
        /**
         * You might be surprised what effects may cause
         * unsychronized calls to printf from different threads.
         * But synchronization is a subject of different week,
         * so far we just block interrupts while we printing
         * something.
         **/
        const int enabled = local_int_save();

        kprintf("%d", id);
        local_int_restore(enabled);
        count--;
    }
    return 0;
}

static int thread0(void* unused) {
    (void)unused;
    for (uint64_t i = 0; i != 100000000; i++)
        ;
    kprintf("Thread0: return 42\n");
    return 42;
}

static int init(void* unused) {
    (void)unused;

    thread_t* thread = thread_create(&thread0, 0);
    int ret;
    thread_start(thread);
    thread_join(thread, &ret);
    thread_destroy(thread);
    kprintf("Thread0 returned %d\n", ret);

    for (uint64_t i = 0; i != 100000000; ++i)
        ;

    thread_t* thread1 = thread_create(&threadx, (void*)1);
    thread_t* thread2 = thread_create(&threadx, (void*)2);

    thread_start(thread1);
    thread_start(thread2);

    while (1) {
        const int enabled = local_int_save();

        // kprintf("I'm thread 0\n");
        // kprintf("0");
        local_int_restore(enabled);
    }
    return 0;
}

void detect_cpu() {
    int ret = support_cpuid();
    kprintf("Support cpuid = %x\n", ret);
    if (ret != 0) {
        cpu_detect();
    }
}

void main(uintptr_t mb_info_phys) {
    terminal_initialize();

    const struct multiboot_info* info =
            (const struct multiboot_info*)va(mb_info_phys);

    balloc_setup(info);
    paging_setup();
    buddy_setup();
    acpi_init();
    ints_setup();
    // pci_init();

    scheduler_setup();
    // test_mapping();
    // buddy_test();

    thread_t* thread = thread_create(&init, 0);
    if (!thread) {
        kprintf("failed to create init thread\n");
        while (1)
            ;
    }
    thread_start(thread);
    /**
     * @brief this function will open interrupt
     *
     */
    scheduler_idle();
    kprintf("end kmain\n");
    // while (1)
    //     ;
}