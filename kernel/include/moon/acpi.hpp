/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 2nd December 2020 11:41:37 am                      *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#pragma once

#include <acpispec/tables.h>
#include <lai/helpers/pci.h>
#include <stdint.h>

struct ACPI_MADT {
  acpi_header_t header;
  uint32_t localAPICAddress;
  uint32_t flags;
} __attribute__((packed));

struct ACPI_MADT_Entry {
  uint8_t type;
  uint8_t length;
} __attribute__((packed));

/**
 * @brief Local APIC
 *  Type = 0
 */
struct LocalAPIC {
  /**
   * @brief MADT Entry structre
   *
   */
  ACPI_MADT_Entry entry;
  /**
   * @brief ACPI Processor ID
   *
   */
  uint8_t processorID;
  /**
   * @brief APIC ID
   *
   */
  uint8_t apicID;
  /**
   * @brief Flags - (bit 0 = enabled, bit 1 = online capable)
   *
   */
  uint32_t flags;
} __attribute__((packed));

/**
 * @brief I/O APIC
 *  Type = 1
 */
struct IOAPIC {
  ACPI_MADT_Entry entry;
  // APIC ID
  uint8_t apicID;
  // reserved
  uint8_t reserved;
  // Address of I/O APIC
  uint32_t address;
  // global system interrupt base
  uint32_t gSIB;
} __attribute__((packed));

/**
 * @brief Interrupt Source Override
 *   Type = 2
 */
struct ISO {
  ACPI_MADT_Entry entry;
  // Bus Source
  uint8_t busSource;
  // IRQ Source
  uint8_t irqSource;
  // Global System Interrupt
  uint8_t gSI;
  // flags
  uint16_t flags;
} __attribute__((packed));

/**
 * @brief
 *
 */
struct NMI {
  ACPI_MADT_Entry entry;
  // ACPI processor ID (255 for all processors)
  uint8_t processorID;
  uint16_t flags;
  // LINT Number (0 or 1)
  uint8_t lINT;
} __attribute__((packed));

/**
 * @brief Local APIC Address Override
 *  Type = 5
 */
struct LocalAPICAddressOverride {
  ACPI_MADT_Entry entry;
  uint16_t reserved;
  // Address of Local APIC
  uint64_t address;
} __attribute__((packed));

struct PCI_MCFG_BaseAddress {
  // Base address of configuration space
  uint64_t baseAddress;
  // PCI Segment group number
  uint16_t segmentGroupNumber;
  // Start PCI bus number decoded by this host bridge
  uint8_t startPCIBusNumber;
  // End PCI bus number decoded by this host bridge
  uint8_t endPCIBusNumber;
  uint32_t reserved;
} __attribute__((packed));

struct PCI_MCFG {
  acpi_header_t header;
  uint64_t reserved;
  PCI_MCFG_BaseAddress baseAddresses[];
} __attribute__((packed));

class ACPI final {
 public:
  ACPI() = delete;
  ~ACPI() = delete;

  static void SetRSDP(acpi_xsdp_t* p);

  static uint8_t RoutePCIPin(uint8_t bus, uint8_t slot, uint8_t func,
                             uint8_t pin);
  static void Init();
  static void Reset();

 private:
  void* FindSDT(const char* signature, int index);

 private:
  static uint8_t fProcessors[];
  static int fProcessorCount;
  static const char* fSignature;
  static acpi_xsdp_t* fDesc;
  static acpi_rsdt_t* fRSDTHeader;
  static acpi_xsdt_t* fXSDTHeader;
  static acpi_fadt_t* fFadt;
  static PCI_MCFG* fMCFG;
  static char fOEM[];
};
