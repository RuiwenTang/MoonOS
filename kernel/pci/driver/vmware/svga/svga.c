#include <moonos/ioport.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <moonos/pci/driver/vmware/svga/svga.h>
#include <moonos/pci/pci.h>
#include <string.h>

svga_device_t gSVGA;

/*
 *-----------------------------------------------------------------------------
 *
 * SVGAFIFOFull --
 *
 *      This function is called repeatedly as long as the FIFO has too
 *      little free space for us to continue.
 *
 *      The simplest implementation of this function is a no-op.  This
 *      will just burn guest CPU until space is available. (That's a
 *      bad idea, since the host probably needs that CPU in order to
 *      make progress on emptying the FIFO.)
 *
 *      A better implementation would sleep until a FIFO progress
 *      interrupt occurs. Depending on the OS you're writing drivers
 *      for, this may deschedule the calling task or it may simply put
 *      the CPU to sleep.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */
static void svga_fifo_full(void);

void svga_init(void) {
    if (!pci_find_device(PCI_VENDOR_ID_VMWARE, PCI_DEVICE_ID_VMWARE_SVGA2,
                         &gSVGA.pci_addr)) {
        kprintf("No VMWare SVGA device found.");
        while (1)
            ;
    }
    /*
     * Use the default base address for each memory region.
     * We must map at least ioBase before using ReadReg/WriteReg.
     */
    pci_set_mem_enable(&gSVGA.pci_addr, 1);
    gSVGA.io_base = pci_get_BAR(&gSVGA.pci_addr, 0);
    gSVGA.fb_mem = (uint8_t*)va(pci_get_BAR(&gSVGA.pci_addr, 1));
    gSVGA.fifo_mem = (uint32_t*)va(pci_get_BAR(&gSVGA.pci_addr, 2));
    kprintf("io_base = %x | fb_mem = %x | fifo_mem = %x \n", gSVGA.io_base,
            gSVGA.fb_mem, gSVGA.fifo_mem);
    /*
     * Version negotiation:
     *
     *   1. Write to SVGA_REG_ID the maximum ID supported by this driver.
     *   2. Read from SVGA_REG_ID
     *      a. If we read back the same value, this ID is supported. We're done.
     *      b. If not, decrement the ID and repeat.
     */
    gSVGA.device_version_id = SVGA_ID_2;
    do {
        svga_write_reg(SVGA_REG_ID, gSVGA.device_version_id);
        if (svga_read_reg(SVGA_REG_ID) == gSVGA.device_version_id) {
            break;
        } else {
            gSVGA.device_version_id--;
        }
    } while (gSVGA.device_version_id >= SVGA_ID_0);

    if (gSVGA.device_version_id < SVGA_ID_0) {
        kprintf("Error negotiating SVGA device version.");
    } else {
        kprintf("device version id = %x\n", gSVGA.device_version_id);
    }

    /*
     * We must determine the FIFO and FB size after version
     * negotiation, since the default version (SVGA_ID_0)
     * does not support the FIFO buffer at all.
     */
    gSVGA.vram_size = svga_read_reg(SVGA_REG_VRAM_SIZE);
    gSVGA.fb_size = svga_read_reg(SVGA_REG_FB_SIZE);
    gSVGA.fifo_size = svga_read_reg(SVGA_REG_MEM_SIZE);

    kprintf("varm_size = %x | fb_size = %x | fifo_size = %x \n",
            gSVGA.vram_size, gSVGA.fb_size, gSVGA.fifo_size);

    /*
     * If the device is new enough to support capability flags, get the
     * capabilities register.
     */
    if (gSVGA.device_version_id >= SVGA_ID_1) {
        gSVGA.capabilities = svga_read_reg(SVGA_REG_CAPABILITIES);
        kprintf("capabilities flags = %x \n", gSVGA.capabilities);
    }

    svga_enable();
}

void svga_enable(void) {
    /*
     * Initialize the command FIFO. The beginning of FIFO memory is
     * used for an additional set of registers, the "FIFO registers".
     * These are higher-performance memory mapped registers which
     * happen to live in the same space as the FIFO. The driver is
     * responsible for allocating space for these registers, according
     * to the maximum number of registers supported by this driver
     * release.
     */
    gSVGA.fifo_mem[SVGA_FIFO_MIN] = SVGA_FIFO_NUM_REGS * sizeof(uint32_t);
    gSVGA.fifo_mem[SVGA_FIFO_MAX] = gSVGA.fifo_size;
    gSVGA.fifo_mem[SVGA_FIFO_NEXT_CMD] = gSVGA.fifo_mem[SVGA_FIFO_MIN];
    gSVGA.fifo_mem[SVGA_FIFO_STOP] = gSVGA.fifo_mem[SVGA_FIFO_MIN];

    /*
     * Prep work for 3D version negotiation. See SVGA3D_Init for
     * details, but we have to give the host our 3D protocol version
     * before enabling the FIFO.
     */
    if (svga_has_fifo_cap(SVGA_CAP_EXTENDED_FIFO) &&
        svga_is_fifo_reg_valid(SVGA_FIFO_GUEST_3D_HWVERSION)) {
        // gSVGA.fifo_mem[SVGA_FIFO_GUEST_3D_HWVERSION] = SVGA3D
        kprintf("svga device support 3D\n");
    } else {
        kprintf("svga device not support 3D\n");
    }

    /*
     * Enable the SVGA device and FIFO.
     */

    svga_write_reg(SVGA_REG_ENABLE, 1);
    svga_write_reg(SVGA_REG_CONFIG_DONE, 1);
}

void svga_set_mode(uint32_t width, uint32_t height, uint32_t bpp) {
    gSVGA.width = width;
    gSVGA.height = height;
    gSVGA.bpp = bpp;

    svga_write_reg(SVGA_REG_WIDTH, width);
    svga_write_reg(SVGA_REG_HEIGHT, height);
    svga_write_reg(SVGA_REG_BITS_PER_PIXEL, bpp);
    svga_write_reg(SVGA_REG_ENABLE, 1);

    gSVGA.pitch = svga_read_reg(SVGA_REG_BYTES_PER_LINE);
}

uint32_t svga_read_reg(uint32_t index) {
    out32(gSVGA.io_base + SVGA_INDEX_PORT, index);
    return in32(gSVGA.io_base + SVGA_VALUE_PORT);
}

void svga_write_reg(uint32_t index, uint32_t value) {
    out32(gSVGA.io_base + SVGA_INDEX_PORT, index);
    out32(gSVGA.io_base + SVGA_VALUE_PORT, value);
}

int svga_is_fifo_reg_valid(int reg) {
    return gSVGA.fifo_mem[SVGA_FIFO_MIN] > (uint32_t)(reg << 2);
}

int svga_has_fifo_cap(int cap) {
    return (gSVGA.fifo_mem[SVGA_FIFO_CAPABILITIES] & cap) != 0;
}

uintptr_t svga_fifo_reserve(uint32_t bytes) {
    volatile uint32_t* fifo = gSVGA.fifo_mem;
    uint32_t max = fifo[SVGA_FIFO_MAX];
    uint32_t min = fifo[SVGA_FIFO_MIN];
    uint32_t next_cmd = fifo[SVGA_FIFO_NEXT_CMD];
    int reserve_able = svga_has_fifo_cap(SVGA_FIFO_CAP_RESERVE);

    /*
     * This example implementation uses only a statically allocated
     * buffer.  If you want to support arbitrarily large commands,
     * dynamically allocate a buffer if and only if it's necessary.
     */
    if (bytes > sizeof gSVGA.fifo.bounce_buffer || bytes > (max - min)) {
        kprintf("FIFO command too large\n");
        while (1)
            ;
    }
    if (bytes % sizeof(uint32_t)) {
        kprintf("FIFO command length not 32-bit aligned\n");
        while (1)
            ;
    }

    if (gSVGA.fifo.reserved_size != 0) {
        kprintf("FIFO reserve before fifo commit\n");
        while (1)
            ;
    }

    gSVGA.fifo.reserved_size = bytes;

    while (1) {
        uint32_t stop = fifo[SVGA_FIFO_STOP];
        int reserve_in_place = 0;
        int need_bounce = 0;

        /*
         * Find a strategy for dealing with "bytes" of data:
         * - reserve in place, if there's room and the FIFO supports it
         * - reserve in bounce buffer, if there's room in FIFO but not
         *   contiguous or FIFO can't safely handle reservations
         * - otherwise, sync the FIFO and try again.
         */
        if (next_cmd >= stop) {
            /* There is no valid FIFO data between nextCmd and max */
            if (next_cmd + bytes < max ||
                (next_cmd + bytes == max && stop > min)) {
                /*
                 * Fastest path 1: There is already enough contiguous space
                 * between nextCmd and max (the end of the buffer).
                 *
                 * Note the edge case: If the "<" path succeeds, we can
                 * quickly return without performing any other tests. If
                 * we end up on the "==" path, we're writing exactly up to
                 * the top of the FIFO and we still need to make sure that
                 * there is at least one unused DWORD at the bottom, in
                 * order to be sure we don't fill the FIFO entirely.
                 *
                 * If the "==" test succeeds, but stop <= min (the FIFO
                 * would be completely full if we were to reserve this
                 * much space) we'll end up hitting the FIFOFull path below.
                 */
                reserve_in_place = 1;
            } else if ((max - next_cmd) + (stop - min) <= bytes) {
                /*
                 * We have to split the FIFO command into two pieces,
                 * but there still isn't enough total free space in
                 * the FIFO to store it.
                 *
                 * Note the "<=". We need to keep at least one DWORD
                 * of the FIFO free at all times, or we won't be able
                 * to tell the difference between full and empty.
                 */
                svga_fifo_full();
            } else {
                /*
                 * Data fits in FIFO but only if we split it.
                 * Need to bounce to guarantee contiguous buffer.
                 */
                need_bounce = 1;
            }
        } else {
            /* There is FIFO data between nextCmd and max */
            if (next_cmd + bytes < stop) {
                /*
                 * Fastest path 2: There is already enough contiguous space
                 * between nextCmd and stop.
                 */
                reserve_in_place = 1;
            } else {
                /*
                 * There isn't enough room between nextCmd and stop.
                 * The FIFO is too full to accept this command.
                 */
                svga_fifo_full();
            }
        }

        /*
         * If we decided we can write directly to the FIFO, make sure
         * the VMX can safely support this.
         */
        if (reserve_in_place) {
            if (reserve_able || bytes <= sizeof(uint32_t)) {
                gSVGA.fifo.unsing_bounce_buffer = 0;
                if (reserve_able) {
                    fifo[SVGA_FIFO_RESERVED] = bytes;
                }
                return (uintptr_t)(next_cmd + (uint8_t*)fifo);
            } else {
                /*
                 * Need to bounce because we can't trust the VMX to safely
                 * handle uncommitted data in FIFO.
                 */
                need_bounce = 1;
            }
        }

        /*
         * If we reach here, either we found a full FIFO, called
         * SVGAFIFOFull to make more room, and want to try again, or we
         * decided to use a bounce buffer instead.
         */
        if (need_bounce) {
            gSVGA.fifo.unsing_bounce_buffer = 1;
            return (uintptr_t)(gSVGA.fifo.bounce_buffer);
        }
    } /* while(1) */
}

uintptr_t svga_fifo_reserve_cmd(uint32_t type, uint32_t bytes) {
    uint32_t* cmd = (uint32_t*)svga_fifo_reserve(bytes + sizeof type);
    cmd[0] = type;
    return (uintptr_t)(cmd + 1);
}

uintptr_t svga_fifo_reserve_escape(uint32_t nsid, uint32_t bytes) {
    uint32_t padded_bytes = (bytes + 3) & ~3UL;
    typedef struct {
        uint32_t cmd;
        uint32_t nsid;
        uint32_t size;
    } __attribute__((__packed__)) svga_cmd_header_t;
    svga_cmd_header_t* header = (svga_cmd_header_t*)svga_fifo_reserve(
            padded_bytes + sizeof *header);

    header->cmd = SVGA_CMD_ESCAPE;
    header->nsid = nsid;
    header->size = bytes;

    return (uintptr_t)(header + 1);
}

void svga_fifo_commit(uint32_t bytes) {
    volatile uint32_t* fifo = gSVGA.fifo_mem;
    uint32_t next_cmd = fifo[SVGA_FIFO_NEXT_CMD];
    uint32_t max = fifo[SVGA_FIFO_MAX];
    uint32_t min = fifo[SVGA_FIFO_MIN];
    int reserve_able = svga_has_fifo_cap(SVGA_FIFO_CAP_RESERVE);

    if (gSVGA.fifo.reserved_size == 0) {
        kprintf("FIFO commit before fifo reserve\n");
        while (1)
            ;
    }
    gSVGA.fifo.reserved_size = 0;
    if (gSVGA.fifo.unsing_bounce_buffer) {
        /*
         * Slow paths: copy out of a bounce buffer.
         */
        uint8_t* buffer = gSVGA.fifo.bounce_buffer;
        if (reserve_able) {
            /*
             * Slow path: bulk copy out of a bounce buffer in two chunks.
             *
             * Note that the second chunk may be zero-length if the reserved
             * size was large enough to wrap around but the commit size was
             * small enough that everything fit contiguously into the FIFO.
             *
             * Note also that we didn't need to tell the FIFO about the
             * reservation in the bounce buffer, but we do need to tell it
             * about the data we're bouncing from there into the FIFO.
             */
            uint32_t chunk_size = MIN(bytes, max - next_cmd);
            fifo[SVGA_FIFO_RESERVED] = bytes;
            memcpy(next_cmd + (uint8_t*)fifo, buffer, chunk_size);
            memcpy(min + (uint8_t*)fifo, buffer + chunk_size,
                   bytes - chunk_size);
        } else {
            /*
             * Slowest path: copy one dword at a time, updating NEXT_CMD as
             * we go, so that we bound how much data the guest has written
             * and the host doesn't know to checkpoint.
             */
            uint32_t* dword = (uint32_t*)buffer;
            while (bytes > 0) {
                fifo[next_cmd / sizeof *dword] = *dword++;
                next_cmd += sizeof *dword;
                if (next_cmd == max) {
                    next_cmd = min;
                }
                fifo[SVGA_FIFO_NEXT_CMD] = next_cmd;
                bytes -= sizeof *dword;
            }
        }
    }

    /*
     * Atomically update NEXT_CMD, if we didn't already
     */
    if (!gSVGA.fifo.unsing_bounce_buffer || reserve_able) {
        next_cmd += bytes;
        if (next_cmd >= max) {
            next_cmd -= max - min;
        }
        fifo[SVGA_FIFO_NEXT_CMD] = next_cmd;
    }

    /*
     * Clear the reservation in the FIFO.
     */
    if (reserve_able) {
        fifo[SVGA_FIFO_RESERVED] = 0;
    }
}

void svga_fifo_commit_all(void) { svga_fifo_commit(gSVGA.fifo.reserved_size); }

uint32_t svga_insert_fence(void) {
    uint32_t fence;
    typedef struct {
        uint32_t id;
        uint32_t fence;
    } __attribute__((__packed__)) svga_cmd_t;
    svga_cmd_t* cmd;

    if (!svga_has_fifo_cap(SVGA_FIFO_CAP_FENCE)) {
        return 1;
    }

    if (gSVGA.fifo.next_fence == 0) {
        gSVGA.fifo.next_fence = 1;
    }
    fence = gSVGA.fifo.next_fence++;

    cmd = (svga_cmd_t*)svga_fifo_reserve(sizeof *cmd);
    cmd->id = SVGA_CMD_FENCE;
    cmd->fence = fence;
    svga_fifo_commit_all();

    return fence;
}

void svga_sync_to_fence(uint32_t fence) {
    if (!fence) {
        return;
    }

    if (!svga_has_fifo_cap(SVGA_FIFO_CAP_FENCE)) {
        /*
         * Fall back on the legacy sync if the host does not support
         * fences.  This is the old sync mechanism that has been
         * supported in the SVGA device pretty much since the dawn of
         * time: write to the SYNC register, then read from BUSY until
         * it's nonzero. This will drain the entire FIFO.
         *
         * The parameter we write to SVGA_REG_SYNC is an arbitrary
         * nonzero value which can be used for debugging, but which is
         * ignored by release builds of VMware products.
         */
        svga_write_reg(SVGA_REG_SYNC, 1);
        while (svga_read_reg(SVGA_REG_BUSY) != 0)
            ;
        return;
    }

    if (svga_has_fence_passed(fence)) {
        /**
         * Nothing to do
         */
        return;
    }

    if (svga_is_fifo_reg_valid(SVGA_FIFO_FENCE_GOAL) &&
        (gSVGA.capabilities & SVGA_CAP_IRQMASK)) {
        /*
         * On hosts which support interrupts and which support the
         * FENCE_GOAL interrupt, we can use our preferred
         * synchronization mechanism.
         *
         * This provides low latency notification both from guest to
         * host and from host to guest, and it doesn't block the guest
         * while we're waiting.
         *
         * This will only work on Workstation 6.5 virtual machines
         * or later. Older virtual machines did not allocate an IRQ
         * for the SVGA device, and the IRQMASK capability will be
         * unset.
         */

        /*
         * Set the fence goal. This asks the host to send an interrupt
         * when this specific fence has been reached.
         */

        while (1)
            ;

    } else {
        /*
         * Sync-to-fence mechanism for older hosts. Wake up the host,
         * and spin on BUSY until we've reached the fence. This
         * processes FIFO commands synchronously, blocking the VM's
         * execution entirely until it's done.
         */
        int busy = 1;
        svga_write_reg(SVGA_REG_SYNC, 1);

        while (!svga_has_fence_passed(fence) && busy) {
            busy = (svga_read_reg(SVGA_REG_BUSY) != 0);
        }
    }

    if (!svga_has_fence_passed(fence)) {
        /*
         * This shouldn't happen. If it does, there might be a bug in
         * the SVGA device.
         */
        kprintf("SyncToFence failed!\n");
        while (1)
            ;
    }
}

int svga_has_fence_passed(uint32_t fence) {
    if (!fence) {
        return 1;
    }

    if (!svga_has_fifo_cap(SVGA_FIFO_CAP_FENCE)) {
        return 0;
    }

    return ((int)(gSVGA.fifo_mem[SVGA_FIFO_FENCE] - fence)) >= 0;
}

void svga_update(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    SVGAFifoCmdUpdate* cmd = (SVGAFifoCmdUpdate*)svga_fifo_reserve_cmd(
            SVGA_CMD_UPDATE, sizeof *cmd);
    cmd->x = x;
    cmd->y = y;
    cmd->width = width;
    cmd->height = height;
    svga_fifo_commit_all();
}

void svga_fifo_full(void) {
    /*
     * Fallback implementation: Perform one iteration of the
     * legacy-style sync. This synchronously processes FIFO commands
     * for an arbitrary amount of time, then returns control back to
     * the guest CPU.
     */
    svga_write_reg(SVGA_REG_SYNC, 1);
    svga_read_reg(SVGA_REG_BUSY);
}