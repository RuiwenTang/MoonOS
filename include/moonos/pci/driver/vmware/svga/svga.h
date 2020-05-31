#ifndef __SVGA_H__
#define __SVGA_H__

#include <moonos/pci/driver/vmware/svga/svga_reg.h>
#include <moonos/pci/pci.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SVGADevice {
    pci_address_t pci_addr;
    uint32_t io_base;
    uint32_t* fifo_mem;
};

#ifdef __cplusplus
}
#endif

#endif  // __SVGA_H__