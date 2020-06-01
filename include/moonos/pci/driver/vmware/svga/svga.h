#ifndef __SVGA_H__
#define __SVGA_H__

#include <moonos/pci/driver/vmware/svga/svga_reg.h>
#include <moonos/pci/pci.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct SVGADevice {
    pci_address_t pci_addr;
    uint32_t io_base;
    uint32_t* fifo_mem;
    uint8_t* fb_mem;
    uint32_t fifo_size;
    uint32_t fb_size;
    uint32_t vram_size;

    uint32_t device_version_id;
    uint32_t capabilities;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t pitch;

    struct {
        uint32_t reserved_size;
        uint8_t unsing_bounce_buffer;
        uint8_t bounce_buffer[1024 * 1024];
        uint32_t next_fence;
    } fifo;

    volatile struct {
        uint32_t pending;
        uint32_t switch_context;
        void* old_context;
        void* new_context;
        uint32_t count;
    } irq;
} svga_device_t;

extern svga_device_t gSVGA;

/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_Init --
 *
 *      Initialize the global SVGA device. This locates it on the PCI bus,
 *      negotiates device version, and maps the command FIFO and framebuffer
 *      memory.
 *
 *      Intr_Init() must have already been called. If the SVGA device
 *      supports interrupts, this will initalize them.
 *
 *      Does not switch video modes.
 *
 * Results:
 *      void.
 *
 * Side effects:
 *      Steals various IOspace and memory regions.
 *      In this example code they're constant addresses, but in reality
 *      you'll need to negotiate these with the operating system.
 *
 *-----------------------------------------------------------------------------
 */
void svga_init(void);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_Enable --
 *
 *      Enable the SVGA device along with the SVGA FIFO.
 *
 * Results:
 *      void.
 *
 * Side effects:
 *      Initializes the command FIFO.
 *
 *-----------------------------------------------------------------------------
 */
void svga_enable(void);
void svga_set_mode(uint32_t width, uint32_t height, uint32_t bpp);
void svga_disable(void);

uint32_t svga_read_reg(uint32_t index);
void svga_write_reg(uint32_t index, uint32_t value);

int svga_is_fifo_reg_valid(int reg);
int svga_has_fifo_cap(int cap);

/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_FIFOReserve --
 *
 *      Begin writing a command to the FIFO buffer. There are several
 *      examples floating around which show how to write to the FIFO
 *      buffer, but this is the preferred method: write directly to
 *      FIFO memory in the common case, but if the command would not
 *      be contiguous, use a bounce buffer.
 *
 *      This method is easy to use, and quite fast. The X.org driver
 *      does not yet use this method, but recent Windows drivers use
 *      it.
 *
 *      The main principles here are:
 *
 *        - There are multiple code paths. In the best case, we write
 *          directly to the FIFO. In the next-best case, we use a
 *          static bounce buffer.  If you need to support arbitrarily
 *          large commands, you can have a worst case in which you use
 *          a dynamically sized bounce buffer.
 *
 *        - We must tell the host that we're reserving FIFO
 *          space. This is important because the device doesn't
 *          guarantee it will preserve the contents of FIFO memory
 *          which hasn't been reserved. If we write to a totally
 *          unused portion of the FIFO and the VM is suspended, on
 *          resume that data will no longer exist.
 *
 *      This function is not re-entrant. If your driver is
 *      multithreaded or may be used from multiple processes
 *      concurrently, you must make sure to serialize all FIFO
 *      commands.
 *
 *      The caller must pair this command with SVGA_FIFOCommit or
 *      SVGA_FIFOCommitAll.
 *
 * Results:
 *      Returns a pointer to the location where the FIFO command can
 *      be written. There will be room for at least 'bytes' bytes of
 *      data.
 *
 * Side effects:
 *      Begins a FIFO command, reserves space in the FIFO.
 *      May block (in SVGAFIFOFull) if the FIFO is full.
 *
 *-----------------------------------------------------------------------------
 */
uintptr_t svga_fifo_reserve(uint32_t bytes);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_FIFOReserveCmd --
 *
 *      This is a convenience wrapper around SVGA_FIFOReserve, which
 *      prefixes the reserved memory block with a uint32 that
 *      indicates the command type.
 *
 * Results:
 *      Always returns a pointer to 'bytes' bytes of reserved space in the FIFO.
 *
 * Side effects:
 *      Begins a FIFO command, reserves space in the FIFO. Writes a
 *      1-word header into the FIFO.  May block (in SVGAFIFOFull) if
 *      the FIFO is full.
 *
 *-----------------------------------------------------------------------------
 */
uintptr_t svga_fifo_reserve_cmd(uint32_t type, uint32_t bytes);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_FIFOReserveEscape --
 *
 *      This is a convenience wrapper around SVGA_FIFOReserve, which
 *      prefixes the reserved memory block with an ESCAPE command header.
 *
 *      ESCAPE commands are a way of encoding extensible and
 *      variable-length packets within the basic FIFO protocol
 *      itself. ESCAPEs are used for some SVGA device functionality,
 *      like video overlays, for VMware's internal debugging tools,
 *      and for communicating with third party code that can load into
 *      the SVGA device.
 *
 * Results:
 *      Always returns a pointer to 'bytes' bytes of reserved space in the FIFO.
 *
 * Side effects:
 *      Begins a FIFO command, reserves space in the FIFO. Writes a
 *      3-word header into the FIFO.  May block (in SVGAFIFOFull) if
 *      the FIFO is full.
 *
 *-----------------------------------------------------------------------------
 */
uintptr_t svga_fifo_reserve_escape(uint32_t nsid, uint32_t bytes);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_FIFOCommit --
 *
 *      Commit a block of FIFO data which was placed in the buffer
 *      returned by SVGA_FIFOReserve. Every Reserve must be paired
 *      with exactly one Commit, but the sizes don't have to match.
 *      The caller is free to commit less space than they
 *      reserved. This can be used if the command size isn't known in
 *      advance, but it is reasonable to make a worst-case estimate.
 *
 *      The commit size does not have to match the size of a single
 *      FIFO command. This can be used to write a partial command, or
 *      to write multiple commands at once.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */
void svga_fifo_commit(uint32_t bytes);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_FIFOCommitAll --
 *
 *      This is a convenience wrapper for SVGA_FIFOCommit(), which
 *      always commits the last reserved block in its entirety.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      SVGA_FIFOCommit.
 *
 *-----------------------------------------------------------------------------
 */
void svga_fifo_commit_all(void);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_InsertFence --
 *
 *      Write a new fence value to the FIFO.
 *
 *      Fences are the basis of the SVGA device's synchronization
 *      model.  A fence is a marker inserted into the FIFO by the
 *      guest. The host processes all FIFO commands in order. Once the
 *      fence is reached, the host does two things:
 *
 *       - The fence value is written to the SVGA_FIFO_FENCE register.
 *       - Optionally, an interrupt is raised.
 *
 *      There are multiple ways to use fences for synchronization. See
 *      SVGA_SyncToFence and SVGA_HasFencePassed.
 *
 * Results:
 *
 *      Returns the value of the fence we inserted. Fence values
 *      increment, wrapping around at 32 bits. Fence value zero is
 *      reserved to mean "no fence". This function never returns zero.
 *
 *      Certain very old versions of the VMware SVGA device do not
 *      support fences. On these devices, we always return 1. On these
 *      devices, SyncToFence will always do a full Sync and
 *      SyncToFence will always return FALSE, so the actual fence
 *      value we use is unimportant. Code written to use fences will
 *      run inefficiently, but it will still be correct.
 *
 * Side effects:
 *      Writes to the FIFO. Increments our nextFence.
 *
 *-----------------------------------------------------------------------------
 */
uint32_t svga_insert_fence(void);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_SyncToFence --
 *
 *      Sleep until the SVGA device has processed all FIFO commands
 *      prior to the insertion point of the specified fence.
 *
 *      This is the most important way to maintain synchronization
 *      between the driver and the SVGA3D device itself. It can be
 *      used to provide flow control between the host and the guest,
 *      or to ensure that DMA operations have completed before reusing
 *      guest memory.
 *
 *      If the provided fence is zero or it has already passed,
 *      this is a no-op.
 *
 *      If the SVGA device and virtual machine hardware version are
 *      both new enough (Workstation 6.5 or later), this will use an
 *      efficient interrupt-driven mechanism to sleep until just after
 *      the host processes the fence.
 *
 *      If not, this will use a less efficient synchronization
 *      mechanism which may require the host to process significantly
 *      more of the FIFO than is necessary.
 *
 * Results:
 *      void.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */
void svga_sync_to_fence(uint32_t fence);
/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_HasFencePassed --
 *
 *      Test whether the host has processed all FIFO commands prior to
 *      the insertion point of the specified fence.
 *
 *      This function tolerates fence wrap-around, but it will return
 *      the wrong result if 'fence' is more than 2^31 fences old. It
 *      is recommended that callers don't allow fence values to
 *      persist indefinitely. Once we notice that a fence has been
 *      passed, that fence variable should be set to zero so we don't
 *      test it in the future.
 *
 * Results:
 *      TRUE if the fence has been passed,
 *      TRUE if fence==0 (no fence),
 *      FALSE if the fence has not been passed,
 *      FALSE if the SVGA device does not support fences.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */
int svga_has_fence_passed(uint32_t fence);
void svga_ring_door_bell(void);

/* 2D commands */

/*
 *-----------------------------------------------------------------------------
 *
 * SVGA_Update --
 *
 *      Send a 2D update rectangle through the FIFO.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */
void svga_update(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

#ifdef __cplusplus
}
#endif

#endif  // __SVGA_H__