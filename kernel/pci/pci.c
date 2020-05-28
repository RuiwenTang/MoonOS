#include <moonos/kprintf.h>
#include <moonos/pci/pci.h>
#include <stdint.h>

static void pci_visit(uint32_t bus, uint32_t dev, uint32_t func) {
    uint32_t id = PCI_MAKE_ID(bus, dev, func);
}

void pci_init(void) { kprintf("Pci init:\n"); }