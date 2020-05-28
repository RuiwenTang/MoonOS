#include <moonos/cpu/acpi.h>
#include <moonos/ioport.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <string.h>

uint32_t g_acpi_cpu_count;
uint8_t g_acpi_cpu_ids[MAX_CPU_COUNT];

struct RSDP* scan_for_rsdp(char* start, uint32_t length) {
    char* end = start + length;
    for (; start < end; start += 16) {
        if (!memcmp(start, RSDP_SIG, sizeof(RSDP_SIG) - 1)) {
            return (struct RSDP*)start;
        }
    }
    return 0;
}

uint8_t acpi_checksum(uint8_t* table, uint32_t length) {
    uint32_t ret = 0;
    while (length--) {
        ret += *table;
        table++;
    }
    return (uint8_t)(ret % 0x100);
}

void get_acpi_table() {
    uintptr_t rsdp = (uintptr_t)va(RSDP_BASE_1);

    kprintf("try find rsdp at 0x%llx\n", (uint64_t)rsdp);

    struct RSDP* ptr = scan_for_rsdp((char*)rsdp, RSDP_END_1 - RSDP_BASE_1);
    if (ptr == 0) {
        kprintf("Not found rsdp location\n");
    } else {
        uintptr_t rsdp_addr = pa(ptr);
        kprintf("Fount rsdp location at phys addr 0x%llx\n",
                (uint64_t)rsdp_addr);
        uint8_t checksum = acpi_checksum((uint8_t*)ptr, sizeof(struct RSDP));
        kprintf("RSDP checksum = %d \n", checksum);
        if (checksum != 0) {
            kprintf("RSDP checksum check failed\n");
            while (1)
                ;
        }

        // check if it is acpi version 2.0
        struct RSDPV2* ptr_v2 = 0;
        checksum = acpi_checksum((uint8_t*)ptr, sizeof(struct RSDPV2));
        if (checksum == 0) {
            kprintf("This is ACPI v2 \n");
            ptr_v2 = (struct RSDPV2*)ptr;
            (void)ptr_v2;
        } else {
            kprintf("This is ACPI v1\n");
        }
    }
}

static int acpi_parse_rsdp(struct RSDP* rsdp) {
    kprintf("RSDP found\n");

    uint8_t checksum = acpi_checksum((uint8_t*)rsdp, sizeof(struct RSDP));
    if (checksum) {
        kprintf("Checksum failed\n");
        while (1)
            ;
    }

    // print oem
    char oem[7];
    memcpy(oem, rsdp->OEMID, 6);
    oem[6] = '\0';
    kprintf("ACPI oem = %s\n", oem);

    // check version
    uint8_t version = rsdp->revision;
    if (version == 0) {
        kprintf("Version 1\n");
        apic_parse_rsdt((acpi_sdt_header_t*)va(rsdp->rsdt_address));
    } else if (version == 2) {
        kprintf("Version 2\n");
    }
    return 0;
}

void acpi_init(void) {
    uintptr_t rsdp = (uintptr_t)va(RSDP_BASE_1);
    struct RSDP* ptr = scan_for_rsdp((char*)rsdp, RSDP_END_1 - RSDP_BASE_1);
    if (ptr == 0) {
        kprintf("Not found rsdp location\n");
        while (1)
            ;
    }

    acpi_parse_rsdp(ptr);
}

void apic_parse_rsdt(acpi_sdt_header_t* header) {
    uint32_t* ptr = (uint32_t*)(header + 1);
    uint32_t* end = (uint32_t*)((uint8_t*)header + header->length);

    while (ptr < end) {
        uint32_t addr = *ptr++;
        apic_parse_sdt((acpi_sdt_header_t*)((uintptr_t)va(addr)));
    }
}

void apic_parse_sdt(acpi_sdt_header_t* header) {
    char sigStr[5];
    memcpy(sigStr, header->signature, 4);
    sigStr[4] = '\0';

    kprintf("sdt signature = %s\n", sigStr);
    if (memcmp(sigStr, "FACP", 4) == 0) {
        apic_parse_facp((fadt_t*)header);
    } else if (memcmp(sigStr, "APIC", 4) == 0) {
        apic_parse_mdat((madt_t*)header);
    }
}

void apic_parse_facp(fadt_t* fadt) {
    uint8_t checksum = acpi_checksum((uint8_t*)fadt, fadt->header.length);
    if (checksum != 0) {
        kprintf("FADT checksum failed checksum = %x \n", checksum);
        while (1)
            ;
    }

    if (fadt->SMI_CommandPort) {
        // TODO wait for sci_en bit
        // kprintf("facp smi_command port = %x\n",
        //         (uint32_t)fadt->SMI_CommandPort);
        // kprintf("facp acpienable = %x\n", (uint32_t)fadt->AcpiEnable);
        out8((uint16_t)fadt->SMI_CommandPort, fadt->AcpiEnable);
        kprintf("ACPI wait enable\n");
        while ((in16((uint16_t)fadt->PM1aControlBlock) & 1) == 0)
            ;

        kprintf("APIC enabled\n");
    } else {
        kprintf("ACPI already enabled\n");
    }
}

void apic_parse_mdat(madt_t* mdat) {
    kprintf("Local APIC Address = 0x%x\n", mdat->local_apic_addr);

    uint8_t* ptr = (uint8_t*)(mdat + 1);
    uint8_t* end = ((uint8_t*)mdat) + mdat->header.length;

    while (ptr < end) {
        apic_header_t* header = (apic_header_t*)ptr;
        if (header->entry_type == APIC_TYPE_LOCAL_APIC) {
            kprintf("Local APIC HEADER -->");
            apic_local_apic_t* local_apic = (apic_local_apic_t*)ptr;
            kprintf("Found cpu: %d %d %x \n",
                    local_apic->acpi_processor_id,
                    local_apic->apic_id,
                    local_apic->flags);

            if (g_acpi_cpu_count < MAX_CPU_COUNT) {
                g_acpi_cpu_ids[g_acpi_cpu_count] = local_apic->apic_id;
                g_acpi_cpu_count++;
            }
        } else if (header->entry_type == APIC_TYPE_IO_APIC) {
            kprintf("IO APIC HEADER -->");
            apic_io_apic_t* io_apic = (apic_io_apic_t*)ptr;
            kprintf(" IO : %d 0x%08x %d \n",
                    io_apic->io_apic_id,
                    io_apic->io_apic_addr,
                    io_apic->global_system_interrupt_base);
        } else if (header->entry_type == APIC_TYPE_INTERRUPT_OVERRIDE) {
            kprintf("INT OVERRIDE APIC HEADER -->");
            apic_int_override_apic_t* int_override_apic =
                    (apic_int_override_apic_t*)ptr;

            kprintf("override: %d %d %d 0x%04x\n",
                    int_override_apic->bus_source,
                    int_override_apic->irq_source,
                    int_override_apic->global_system_interrupt,
                    int_override_apic->flags);
        }

        ptr += header->record_length;
    }
}