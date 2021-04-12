/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 2nd December 2020 7:19:03 pm                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#include <moon/memory.h>

#include <moon/acpi.hpp>
#include <moon/io.hpp>

#include "umm_malloc.h"

#define DEBUG_ACPI 1

#if DEBUG_ACPI
#include "kprintf.hpp"
#endif

uint8_t ACPI::fProcessors[256];
int ACPI::fProcessorCount = 1;
const char* ACPI::fSignature = "RSD PTR ";
acpi_xsdp_t* ACPI::fDesc = nullptr;
acpi_rsdt_t* ACPI::fRSDTHeader = nullptr;
acpi_xsdt_t* ACPI::fXSDTHeader = nullptr;
acpi_fadt_t* ACPI::fFadt = nullptr;
PCI_MCFG* ACPI::fMCFG = nullptr;
char ACPI::fOEM[7];
algorithm::List<ISO*>* ACPI::fISOList = nullptr;

void* ACPI::FindSDT(const char* signature, int index) {
  int entries = 0;
  if (fDesc->revision == 2) {
    entries = (fRSDTHeader->header.length - sizeof(acpi_header_t)) /
              sizeof(uint64_t);  // ACPI 2.0
  } else {
    entries = (fRSDTHeader->header.length - sizeof(acpi_header_t)) /
              sizeof(uint32_t);  // ACPI 1.0
  }

  auto getEntry = [](unsigned i) -> uintptr_t {
    if (fDesc->revision == 2) {
      return fXSDTHeader->tables[i];
    } else {
      return fRSDTHeader->tables[i];
    }
  };

  int _index = 0;

  if (memcmp("DSDT", signature, 4) == 0) {
    return (void*)VA(fFadt->dsdt);
  }

  for (int i = 0; i < entries; i++) {
    acpi_header_t* h = reinterpret_cast<acpi_header_t*>(VA(getEntry(i)));
    if (memcmp(h->signature, signature, 4) == 0 && _index == index) {
      return h;
    }
  }

  // No SDT found
  return 0;
}

void ACPI::Init() {
  if (fDesc) {
    goto success;
  }

  // search first Kb for RSDP, which is aligned on 16 byte boundary
  // more info see https://wiki.osdev.org/RSDP
  for (int i = 0; i < 0x7BFF; i += 16) {
    if (memcmp((void*)VA(i), fSignature, 8) == 0) {
      fDesc = reinterpret_cast<acpi_xsdp_t*>(VA(i));
#if DEBUG_ACPI
      kprintf("ACPI find RSDP at %x\n", fDesc);
#endif
      goto success;
    }
  }

  for (int i = 0x80000; i <= 0x9FFFF; i += 16) {
    if (memcmp((void*)VA(i), fSignature, 8) == 0) {
      fDesc = reinterpret_cast<acpi_xsdp_t*>(VA(i));
#if DEBUG_ACPI
      kprintf("ACPI find RSDP at %x\n", PA(fDesc));
#endif
      goto success;
    }
  }

  for (int i = 0xE0000; i <= 0xFFFFF; i += 16) {
    if (memcmp((void*)VA(i), fSignature, 8) == 0) {
      fDesc = reinterpret_cast<acpi_xsdp_t*>(VA(i));
#if DEBUG_ACPI
      kprintf("ACPI find RSDP at %x\n", PA(fDesc));
#endif
      goto success;
    }
  }
  // no acpi complaiant
  // TODO panic with this failure

success:

  if (fDesc->revision == 2) {
    fRSDTHeader = reinterpret_cast<acpi_rsdt_t*>(VA(fDesc->xsdt));
    fXSDTHeader = reinterpret_cast<acpi_xsdt_t*>(VA(fDesc->xsdt));
#if DEBUG_ACPI
    kprintf("ACPI with version 2.0\n");
#endif
  } else {
    fRSDTHeader = reinterpret_cast<acpi_rsdt_t*>(VA(fDesc->rsdt));
#if DEBUG_ACPI
    kprintf("ACPI with version 1.0\n");
#endif
  }

  fISOList = new algorithm::List<ISO*>();

  memcpy(fOEM, fRSDTHeader->header.oem, 6);
  fOEM[6] = 0;
#if DEBUG_ACPI
  kprintf("ACPI OEM: %s \n", fOEM);
#endif

  fFadt = reinterpret_cast<acpi_fadt_t*>(FindSDT("FACP", 0));

#if DEBUG_ACPI
  if (fFadt) {
    kprintf("FIND FADT at %x\n", PA(fFadt));
  }
#endif

  // close interrupt
  asm("cli");

  lai_set_acpi_revision(fRSDTHeader->header.revision);
  lai_create_namespace();

  ReadMADT();

  kprintf("proc count = %d\n", fProcessorCount);
  // Attemp to find MCFG table for PCI
  fMCFG = reinterpret_cast<PCI_MCFG*>(FindSDT("MCFG", 0));
  asm("sti");
  return;
}

int ACPI::ReadMADT() {
  void* madt = FindSDT("APIC", 0);
  if (!madt) {
#if DEBUG_ACPI
    kprintf("Could Not Find MADT\n");
#endif
    return -1;
  }

  ACPI_MADT* madtHeader = reinterpret_cast<ACPI_MADT*>(madt);
  uintptr_t madt_end =
      reinterpret_cast<uintptr_t>(madt) + madtHeader->header.length;

  uintptr_t madt_entry = reinterpret_cast<uintptr_t>(madt) + sizeof(ACPI_MADT);

  while (madt_entry < madt_end) {
    ACPI_MADT_Entry* entry = reinterpret_cast<ACPI_MADT_Entry*>(madt_entry);
    switch (entry->type) {
      case 0: {
        LocalAPIC* local_apic = reinterpret_cast<LocalAPIC*>(entry);
        if (local_apic->flags & 0x3) {
          if (local_apic->apicID == 0) {
            // find BSP
            break;
          }
          fProcessors[fProcessorCount++] = local_apic->apicID;
#if DEBUG_ACPI
          kprintf("[ACPI] Found Processor, APIC ID: %d\n", local_apic->apicID);
#endif
        }
      } break;
      case 1: {
        IOAPIC* io_apic = reinterpret_cast<IOAPIC*>(entry);
#if DEBUG_ACPI
        kprintf("[ACPI] Found I/O APIC, Address: %x\n", io_apic->address);
#endif
        // TODO set APIC IO Base
      } break;
      case 2: {
        ISO* interrupt_source_override = reinterpret_cast<ISO*>(entry);
        fISOList->addBack(interrupt_source_override);
      } break;
    }
    madt_entry += entry->length;
  }
  return 0;
}

extern "C" {

void* laihost_scan(const char* signature, size_t index) {
  return ACPI::FindSDT(signature, index);
}

void laihost_log(int level, const char* msg) {
  const char* log_level = nullptr;
  switch (level) {
    case LAI_WARN_LOG:
      log_level = "lai_warning";
      break;
    case LAI_DEBUG_LOG:
      log_level = "lai_debug";
      break;
  }
#if DEBUG_ACPI
  kprintf("%s: ", log_level);
  kprintf("%s\n", msg);
#endif
}

void laihost_panic(const char* msg) {
#if DEBUG_ACPI
  kprintf("ACPI Error: %s\n", msg);
  for (;;)
    ;
#endif
}

void* laihost_malloc(size_t sz) { return umm_malloc(sz); }

void* laihost_realloc(void* addr, size_t sz, size_t /* oldsz */) {
  return umm_realloc(addr, sz);
}

void laihost_free(void* addr, size_t /* sz */) { umm_free(addr); }

void* laihost_map(size_t address, size_t count) {
  // TODO implement kernel map
  (void)address;
  (void)count;
  return nullptr;
}

void laihost_unmap(void* ptr, size_t count) {
  (void)ptr;
  (void)count;
  // stub
}

void laihost_outb(uint16_t port, uint8_t val) { IO::Write8(port, val); }

void laihost_outw(uint16_t port, uint16_t val) { IO::Write16(port, val); }

void laihost_outd(uint16_t port, uint32_t val) { IO::Write32(port, val); }

uint8_t laihost_inb(uint16_t port) { return IO::Read8(port); }

uint16_t laihost_inw(uint16_t port) { return IO::Read16(port); }

uint32_t laihost_ind(uint16_t port) { return IO::Read32(port); }

void laihost_sleep(uint64_t ms) {
  // TODO implement
  (void)ms;
}

void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun,
                        uint16_t offset, uint16_t val) {
  // TODO implement
  (void)seg;
  (void)bus;
  (void)slot;
  (void)fun;
  (void)offset;
  (void)val;
}
}