/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 2nd December 2020 7:19:03 pm                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#include <moon/memory.h>

#include <moon/acpi.hpp>

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
    return (void*)0;
  }

  return 0;
}

void ACPI::Init() {
  if (fDesc) {
    goto success;
  }

  // search first Kb for RSDP, which is aligned on 16 byte boundary
  for (int i = 0; i < 0x7BFF; i += 16) {
    if (memcmp((void*)VA(i), fSignature, 8) == 0) {
      fDesc = reinterpret_cast<acpi_xsdp_t*>(VA(i));
#if DEBUG_ACPI
      kprintf("ACPI find RSDP at %x\n", fDesc);
#endif
      goto success;
    }
  }

success:

  return;
}