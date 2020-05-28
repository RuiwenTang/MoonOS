#ifndef __ACPI_H__
#define __ACPI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RSDP_BASE_1 0xE0000
#define RSDP_END_1 0xFFFFF
#define BIOS_EBDA_PA 0x40E

#define RSDP_SIG "RSD PTR "
#define RSDT_SIG "RSDT"
#define XSDT_SIG "XSDT"
#define APIC_SIG "APIC"

#define MAX_CPU_COUNT 8

extern uint32_t g_acpi_cpu_count;
extern uint8_t g_acpi_cpu_ids[MAX_CPU_COUNT];

struct RSDP {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__((packed));

struct RSDPV2 {
    struct RSDP rsdp;

    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed));

struct ACPISDTHEADER {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));
typedef struct ACPISDTHEADER acpi_sdt_header_t;

struct RSDT {
    acpi_sdt_header_t header;
    uint32_t* pointer_to_other_sdt;
};

struct GenericAddressStructure {
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
} __attribute__((packed));

/**
 * @brief signature FACP
 *
 */
struct FADT {
    acpi_sdt_header_t header;
    uint32_t firmware_control;
    uint32_t dsdt;
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t Reserved;

    uint8_t PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t AcpiEnable;
    uint8_t AcpiDisable;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t PM1EventLength;
    uint8_t PM1ControlLength;
    uint8_t PM2ControlLength;
    uint8_t PMTimerLength;
    uint8_t GPE0Length;
    uint8_t GPE1Length;
    uint8_t GPE1Base;
    uint8_t CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t DutyOffset;
    uint8_t DutyWidth;
    uint8_t DayAlarm;
    uint8_t MonthAlarm;
    uint8_t Century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;

    uint8_t Reserved2;
    uint32_t Flags;

    // 12 byte structure; see below for details
    struct GenericAddressStructure ResetReg;
    uint8_t ResetValue;
    uint8_t Reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;

    struct GenericAddressStructure X_PM1aEventBlock;
    struct GenericAddressStructure X_PM1bEventBlock;
    struct GenericAddressStructure X_PM1aControlBlock;
    struct GenericAddressStructure X_PM1bControlBlock;
    struct GenericAddressStructure X_PM2ControlBlock;
    struct GenericAddressStructure X_PMTimerBlock;
    struct GenericAddressStructure X_GPE0Block;
    struct GenericAddressStructure X_GPE1Block;
} __attribute__((packed));

typedef struct FADT fadt_t;

/**
 * @brief signature APIC
 *
 */
struct MADT {
    acpi_sdt_header_t header;
    uint32_t local_apic_addr;
    uint32_t flags;
} __attribute__((packed));

typedef struct MADT madt_t;

struct APICHEADER {
    uint8_t entry_type;
    uint8_t record_length;
} __attribute__((packed));
typedef struct APICHEADER apic_header_t;

enum apic_header_type {
    APIC_TYPE_LOCAL_APIC,
    APIC_TYPE_IO_APIC,
    APIC_TYPE_INTERRUPT_OVERRIDE,
};

struct APICLOCALAPIC {
    apic_header_t header;
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed));
typedef struct APICLOCALAPIC apic_local_apic_t;

struct APICIOAPIC {
    apic_header_t header;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t global_system_interrupt_base;
} __attribute__((packed));
typedef struct APICIOAPIC apic_io_apic_t;

struct APICINTOVERRIDEAPIC {
    apic_header_t header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed));
typedef struct APICINTOVERRIDEAPIC apic_int_override_apic_t;

struct RSDP* scan_for_rsdp(char* start, uint32_t length);

uint8_t acpi_checksum(uint8_t* table, uint32_t length);

void acpi_init(void);

uint32_t acpi_remap_irq(uint32_t irq);
void apic_parse_rsdt(acpi_sdt_header_t* header);
void apic_parse_sdt(acpi_sdt_header_t* header);
void apic_parse_facp(fadt_t* fadt);
void apic_parse_mdat(madt_t* mdat);

uint32_t apic_remap_irq(uint8_t irq);

#ifdef __cplusplus
}
#endif

#endif  // __ACPI_H__