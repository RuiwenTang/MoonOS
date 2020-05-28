#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PCI_MAKE_ID(bus, dev, func) \
    (((bus) << 16) | ((dev) << 11) | ((func) << 8))

// I/O Ports
#define PCI_CONFIG_ADDR 0xcf8
#define PCI_CONFIG_DATA 0xcfC

// Header Type
#define PCI_TYPE_MULTIFUNC 0x80
#define PCI_TYPE_GENERIC 0x00
#define PCI_TYPE_PCI_BRIDGE 0x01
#define PCI_TYPE_CARDBUS_BRIDGE 0x02

// PCI Configuration Registers
#define PCI_CONFIG_VENDOR_ID 0x00
#define PCI_CONFIG_DEVICE_ID 0x02
#define PCI_CONFIG_COMMAND 0x04
#define PCI_CONFIG_STATUS 0x06
#define PCI_CONFIG_REVISION_ID 0x08
#define PCI_CONFIG_PROG_INTF 0x09
#define PCI_CONFIG_SUBCLASS 0x0a
#define PCI_CONFIG_CLASS_CODE 0x0b
#define PCI_CONFIG_CACHELINE_SIZE 0x0c
#define PCI_CONFIG_LATENCY 0x0d
#define PCI_CONFIG_HEADER_TYPE 0x0e
#define PCI_CONFIG_BIST 0x0f

// Type 0x00 (Generic) Configuration Registers
#define PCI_CONFIG_BAR0 0x10
#define PCI_CONFIG_BAR1 0x14
#define PCI_CONFIG_BAR2 0x18
#define PCI_CONFIG_BAR3 0x1c
#define PCI_CONFIG_BAR4 0x20
#define PCI_CONFIG_BAR5 0x24
#define PCI_CONFIG_CARDBUS_CIS 0x28
#define PCI_CONFIG_SUBSYSTEM_VENDOR_ID 0x2c
#define PCI_CONFIG_SUBSYSTEM_DEVICE_ID 0x2e
#define PCI_CONFIG_EXPANSION_ROM 0x30
#define PCI_CONFIG_CAPABILITIES 0x34
#define PCI_CONFIG_INTERRUPT_LINE 0x3c
#define PCI_CONFIG_INTERRUPT_PIN 0x3d
#define PCI_CONFIG_MIN_GRANT 0x3e
#define PCI_CONFIG_MAX_LATENCY 0x3f

// ------------------------------------------------------------------------------------------------
// PCI BAR

#define PCI_BAR_IO 0x01
#define PCI_BAR_LOWMEM 0x02
#define PCI_BAR_64 0x04
#define PCI_BAR_PREFETCH 0x08

uint8_t pci_read8(uint32_t id, uint32_t reg);
uint16_t pci_read16(uint32_t id, uint32_t reg);
uint32_t pci_read32(uint32_t id, uint32_t reg);

void pci_write8(uint32_t id, uint32_t reg, uint8_t data);
void pci_write16(uint32_t id, uint32_t reg, uint16_t data);
void pci_write32(uint32_t id, uint32_t reg, uint32_t data);

void pci_init(void);

#ifdef __cplusplus
}
#endif

#endif  // __PCI_H__