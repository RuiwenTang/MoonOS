#include <moonos/ioport.h>
#include <moonos/kprintf.h>
#include <moonos/pci/pci.h>
#include <stdint.h>

static void pci_visit(uint32_t bus, uint32_t dev, uint32_t func) {
    uint32_t id = PCI_MAKE_ID(bus, dev, func);

    pci_device_info_t info;
    info.vendor_id = pci_read16(id, PCI_CONFIG_VENDOR_ID);
    if (info.vendor_id == 0xffff) {
        return;
    }
    info.device_id = pci_read16(id, PCI_CONFIG_DEVICE_ID);
    info.progIntf = pci_read8(id, PCI_CONFIG_PROG_INTF);
    info.sub_class = pci_read8(id, PCI_CONFIG_SUBCLASS);
    info.class_code = pci_read8(id, PCI_CONFIG_CLASS_CODE);
    kprintf("pci vendor id = %x | device id = %x |progIF = %x \n",
            info.vendor_id, info.device_id, info.progIntf);
}

void pci_init(void) {
    kprintf("Pci init:\n");
    for (uint32_t bus = 0; bus < 256; bus++) {
        for (uint32_t dev = 0; dev < 32; dev++) {
            uint32_t base_id = PCI_MAKE_ID(bus, dev, 0);
            uint8_t header_type = pci_read8(base_id, PCI_CONFIG_HEADER_TYPE);
            uint32_t func_count = header_type & PCI_TYPE_MULTIFUNC ? 8 : 1;

            for (uint32_t func = 0; func < func_count; func++) {
                pci_visit(bus, dev, func);
            }
        }
    }
}

uint8_t pci_read8(uint32_t id, uint32_t reg) {
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    out32(PCI_CONFIG_ADDR, addr);
    return in8(PCI_CONFIG_DATA + (reg & 0x03));
}

uint16_t pci_read16(uint32_t id, uint32_t reg) {
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    out32(PCI_CONFIG_ADDR, addr);
    return in16(PCI_CONFIG_DATA + (reg & 0x02));
}

uint32_t pci_read32(uint32_t id, uint32_t reg) {
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    out32(PCI_CONFIG_ADDR, addr);
    return in32(PCI_CONFIG_DATA);
}

void pci_write8(uint32_t id, uint32_t reg, uint8_t data) {
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    out32(PCI_CONFIG_ADDR, addr);
    out8(PCI_CONFIG_DATA + (reg & 0x03), data);
}

void pci_write16(uint32_t id, uint32_t reg, uint16_t data) {
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    out32(PCI_CONFIG_ADDR, addr);
    out16(PCI_CONFIG_DATA + (reg & 0x02), data);
}

void pci_write32(uint32_t id, uint32_t reg, uint32_t data) {
    uint32_t addr = 0x80000000 | id | (reg & 0xfc);
    out32(PCI_CONFIG_ADDR, addr);
    out32(PCI_CONFIG_DATA, data);
}

static void pci_read_bar(uint32_t id, uint32_t index, uint32_t* addr,
                         uint32_t* mask) {
    uint32_t reg = PCI_CONFIG_BAR0 + index * sizeof(uint32_t);
    // get address
    *addr = pci_read32(id, reg);

    // find out size of bar
    pci_write32(id, reg, 0xffffffff);
    *mask = pci_read32(id, reg);

    // restore address
    pci_write32(id, reg, *addr);
}

void pci_get_bar(pci_bar_t* bar, uint32_t id, uint32_t index) {
    // read pci bar register
    uint32_t address_low;
    uint32_t mask_low;
    pci_read_bar(id, index, &address_low, &mask_low);

    if (address_low & PCI_BAR_64) {
        // 64 bit mmio
        uint32_t address_high;
        uint32_t mask_high;
        pci_read_bar(id, index + 1, &address_high, &mask_high);

        bar->u.address =
                (((uintptr_t)address_high << 32) | (address_low & ~0xf));
        bar->size = ~(((uint64_t)mask_high << 32) | (mask_low & ~0xf)) + 1;
        bar->flags = address_low & 0xf;
    } else if (address_low & PCI_BAR_IO) {
        // io register
        bar->u.port = (uint16_t)(address_low & ~0x3);
        bar->size = (uint16_t)(~(mask_low & ~0x3) + 1);
        bar->flags = address_low & 0x3;
    } else {
        // 32 bit mmio
        bar->u.address = (uintptr_t)(address_low & ~0xf);
        bar->size = ~(mask_low & ~0xf) + 1;
        bar->flags = address_low & 0xf;
    }
}