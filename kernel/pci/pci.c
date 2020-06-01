#include <moonos/ioport.h>
#include <moonos/kprintf.h>
#include <moonos/pci/driver/vmware/svga/svga.h>
#include <moonos/pci/pci.h>
#include <stdint.h>

/*
 * There can be up to 256 PCI busses, but it takes a noticeable
 * amount of time to scan that whole space. Limit the number of
 * supported busses to something more reasonable...
 */
#define PCI_MAX_BUSSES 0x20

#define PCI_REG_CONFIG_ADDRESS 0xCF8
#define PCI_REG_CONFIG_DATA 0xCFC

static uint32_t pci_config_pack_address(const pci_address_t* addr,
                                        uint16_t offset) {
    const uint32_t enable_bit = 0x80000000UL;

    return (((uint32_t)addr->bus << 16) | ((uint32_t)addr->device << 11) |
            ((uint32_t)addr->function << 8) | offset | enable_bit);
}

uint32_t pci_config_read32(const pci_address_t* addr, uint16_t offset) {
    out32(PCI_REG_CONFIG_ADDRESS, pci_config_pack_address(addr, offset));
    return in32(PCI_REG_CONFIG_DATA);
}

uint16_t pci_config_read16(const pci_address_t* addr, uint16_t offset) {
    out32(PCI_REG_CONFIG_ADDRESS, pci_config_pack_address(addr, offset));
    return in16(PCI_REG_CONFIG_DATA);
}

uint8_t pci_config_read8(const pci_address_t* addr, uint16_t offset) {
    out32(PCI_REG_CONFIG_ADDRESS, pci_config_pack_address(addr, offset));
    return in8(PCI_REG_CONFIG_DATA);
}

void pci_config_write32(const pci_address_t* addr,
                        uint16_t offset,
                        uint32_t data) {
    out32(PCI_REG_CONFIG_ADDRESS, pci_config_pack_address(addr, offset));
    out32(PCI_REG_CONFIG_DATA, data);
}

void pci_config_write16(const pci_address_t* addr,
                        uint16_t offset,
                        uint16_t data) {
    out32(PCI_REG_CONFIG_ADDRESS, pci_config_pack_address(addr, offset));
    out16(PCI_REG_CONFIG_DATA, data);
}

void pci_config_write8(const pci_address_t* addr,
                       uint16_t offset,
                       uint8_t data) {
    out32(PCI_REG_CONFIG_ADDRESS, pci_config_pack_address(addr, offset));
    out8(PCI_REG_CONFIG_DATA, data);
}

int pci_scanbus(pci_scan_state_t* state) {
    pci_config_space_t config;
    for (;;) {
        config.words[0] = pci_config_read32(&state->next_addr, 0);
        state->addr = state->next_addr;
        if (++state->next_addr.function == 0x8) {
            state->next_addr.function = 0;
            if (++state->next_addr.device == 0x20) {
                state->next_addr.device = 0;
                if (++state->next_addr.bus == PCI_MAX_BUSSES) {
                    return 0;
                }
            }
        }

        if (config.words[0] != 0xFFFFFFFFUL) {
            state->vendor_id = config.vendorId;
            state->device_id = config.device_id;
            return 1;
        }
    }
}

int pci_find_device(uint16_t vendor_id,
                    uint16_t device_id,
                    pci_address_t* addr_out) {
    pci_scan_state_t bus_scan = {};

    while (pci_scanbus(&bus_scan)) {
        if (bus_scan.vendor_id == vendor_id &&
            bus_scan.device_id == device_id) {
            *addr_out = bus_scan.addr;
            return 1;
        }
    }
    return 0;
}

void pci_set_BAR(const pci_address_t* addr, int index, uint32_t value) {
    pci_config_write32(addr, offsetof(pci_config_space_t, BAR[index]), value);
}

uint32_t pci_get_BAR(const pci_address_t* addr, int index) {
    uint32_t bar =
            pci_config_read32(addr, offsetof(pci_config_space_t, BAR[index]));

    uint32_t mask = (bar & PCI_CONF_BAR_IO) ? 0x3 : 0xf;
    return bar & ~mask;
}

void pci_set_mem_enable(const pci_address_t* addr, int enable) {
    uint16_t command =
            pci_config_read16(addr, offsetof(pci_config_space_t, command));

    /* Mem space enable, IO space enable, bus mastering. */
    const uint16_t flags = 0x0007;

    if (enable) {
        command |= flags;
    } else {
        command &= ~flags;
    }

    pci_config_write16(addr, offsetof(pci_config_space_t, command), command);
}

static void paint_screen(uint32_t color) {
    uint32_t* fb = (uint32_t*)gSVGA.fb_mem;
    int x, y;
    static uint32_t fence = 0;
    /*
     * Flow control: Before we re-write the beginning of the
     * framebuffer, make sure the host has at least finished drawing
     * the beginning of the last frame.
     *
     * This isn't as strict as it should be: For proper 2D flow
     * control, we really need to either do a full sync at the end of
     * each frame, or double-buffer (alternating buffers on every other
     * frame). This method can produce glitches, but it's still decent
     * for benchmarking purposes.
     */

    svga_sync_to_fence(fence);
    fence = svga_insert_fence();

    /*
     * Update the screen, pixel by pixel.
     */

    for (y = 0; y < gSVGA.height; y++) {
        uint32_t* row = fb;
        fb = (uint32_t*)(gSVGA.pitch + (uint8_t*)fb);

        for (x = 0; x < gSVGA.width; x++) {
            *(row++) = color;
            svga_update(x, y, 1, 1);
        }
    }
}

void pci_init(void) {
    kprintf("pci init ---------- \n");
    svga_init();
    svga_set_mode(1024, 768, 32);
    while (1) {
        paint_screen(0x747cba);
    }
}