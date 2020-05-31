#ifndef __PCI_H__
#define __PCI_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef offsetof
#define offsetof(type, member) ((uint32_t)(&((type*)NULL)->member))
#endif

typedef union PCIConfigSpace {
    uint32_t words[16];
    struct {
        uint16_t vendorId;
        uint16_t device_id;
        uint16_t command;
        uint16_t status;
        uint16_t revision_id;
        uint8_t subclass;
        uint8_t class_code;
        uint8_t cache_line_size;
        uint8_t lat_timer;
        uint8_t header_type;
        uint8_t bist;
        uint32_t BAR[6];
        uint32_t card_bus_cis;
        uint16_t subsys_vendor_id;
        uint16_t subsys_id;
        uint32_t expansion_rom_addr;
        uint32_t reserved0;
        uint32_t reserved1;
        uint8_t intr_line;
        uint8_t intr_pin;
        uint8_t min_grant;
        uint8_t max_latency;
    };
} __attribute__((__packed__)) pci_config_space_t;

typedef struct PCIAddress {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
} pci_address_t;

typedef struct PCIScanState {
    uint16_t vendor_id;
    uint16_t device_id;
    pci_address_t next_addr;
    pci_address_t addr;
} pci_scan_state_t;

// BAR bits
#define PCI_CONF_BAR_IO 0x01
#define PCI_CONF_BAR_64BIT 0x04
#define PCI_CONF_BAR_PREFETCH 0x08

/*
 * PCI_ConfigRead32 --
 * PCI_ConfigRead16 --
 * PCI_ConfigRead8 --
 * PCI_ConfigWrite32 --
 * PCI_ConfigWrite16 --
 * PCI_ConfigWrite8 --
 *
 *    Access a device's PCI configuration space, using configuration
 *    mechanism #1. All new machines should use method #1, method #2
 *    is for legacy compatibility.
 *
 *    See http://www.osdev.org/wiki/PCI
 */

uint32_t pci_config_read32(const pci_address_t* addr, uint16_t offset);
uint16_t pci_config_read16(const pci_address_t* addr, uint16_t offset);
uint8_t pci_config_read8(const pci_address_t* addr, uint16_t offset);

void pci_config_write32(const pci_address_t* addr, uint16_t offset,
                        uint32_t data);
void pci_config_write16(const pci_address_t* addr, uint16_t offset,
                        uint16_t data);
void pci_config_write8(const pci_address_t* addr, uint16_t offset,
                       uint8_t data);

/*
 * PCI_ScanBus --
 *
 *    Scan the PCI bus for devices. Before starting a scan,
 *    the caller should zero the PCIScanState structure.
 *    Every time this function is called, it returns the next
 *    device in sequence.
 *
 *    Returns TRUE if a device was found, leaving that device's
 *    vendorId, productId, and address in 'state'.
 *
 *    Returns FALSE if there are no more devices.
 */
int pci_scanbus(pci_scan_state_t* state);
/*
 * PCI_FindDevice --
 *
 *    Scan the PCI bus for a device with a specific vendor and device ID.
 *
 *    On success, returns TRUE and puts the device address into 'addrOut'.
 *    If the device was not found, returns FALSE.
 */
int pci_find_device(uint16_t vendor_id, uint16_t device_id,
                    pci_address_t* addr_out);
/*
 * PCI_SetBAR --
 *
 *    Set one of a device's Base Address Registers to the provided value.
 */
void pci_set_BAR(const pci_address_t* addr, int index, uint32_t value);
uint32_t pci_get_BAR(const pci_address_t* addr, int index);
/*
 * PCI_SetMemEnable --
 *
 *    Enable or disable a device's memory and IO space. This must be
 *    called to enable a device's resources after setting all
 *    applicable BARs. Also enables/disables bus mastering.
 */
void pci_set_mem_enable(const pci_address_t* addr, int enable);

#ifdef __cplusplus
}
#endif

#endif  // __PCI_H__