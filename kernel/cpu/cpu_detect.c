#include <moonos/cpu/cpuid.h>
#include <moonos/kprintf.h>

void cpu_detect() {
    uint32_t eax, ebx, ecx, edx;

    uint32_t largest_standard_func;
    char vendor[13];

    cpuid(CPUID_GETVENDORSTRING, &largest_standard_func, (uint32_t*)vendor,
          (uint32_t*)(vendor + 8), (uint32_t*)(vendor + 4));
    vendor[12] = '\0';

    kprintf("CPU vendor string = %s\n", vendor);
    // Function 0x01 - Feature Information
    if (largest_standard_func >= 0x01) {
        // get feature
        cpuid(CPUID_GETFEATURES, &eax, &ebx, &ecx, &edx);
        kprintf("CPU Features:\n");
        if (edx & CPUID_FEAT_EDX_PSE) kprintf(" PSE");
        if (edx & CPUID_FEAT_EDX_PAE) kprintf(" PAE");
        if (edx & CPUID_FEAT_EDX_APIC) kprintf(" APIC");
        if (edx & CPUID_FEAT_EDX_MTRR) kprintf(" MTRR");

        kprintf("\n");

        kprintf("Instructions: \n");
        if (edx & CPUID_FEAT_EDX_TSC) kprintf(" TSC");
        if (edx & CPUID_FEAT_EDX_MSR) kprintf(" MSR");
        if (edx & CPUID_FEAT_EDX_SSE) kprintf(" SSE");
        if (edx & CPUID_FEAT_EDX_SSE2) kprintf(" SSE2");
        kprintf("\n");
    }

    // Extended Function 0x00 - Largest Extended Function
    uint32_t largest_extended_func;
    cpuid(CPUID_INTELEXTENDED, &largest_extended_func, &ebx, &ecx, &edx);
    // Extended Function 0x01 - Extended Feature Bits
    if (largest_extended_func >= CPUID_INTELFEATURES) {
        cpuid(CPUID_INTELFEATURES, &eax, &ebx, &ecx, &edx);
        if (edx & CPUID_FEAT_EDX_IA64) {
            kprintf(" 64-bit Architecture\n");
        }
    }

    // Extended Function 0x02-0x04 - Processor Name / Brand String
    if (largest_extended_func >= CPUID_INTELBRANDSTRINGEND) {
        char name[48];
        name[47] = '\0';
        cpuid(CPUID_INTELBRANDSTRING, (uint32_t*)name, (uint32_t*)(name + 4),
              (uint32_t*)(name + 8), (uint32_t*)(name + 12));

        cpuid(CPUID_INTELBRANDSTRINGMORE, (uint32_t*)(name + 16),
              (uint32_t*)(name + 20), (uint32_t*)(name + 24),
              (uint32_t*)(name + 28));

        cpuid(CPUID_INTELBRANDSTRINGEND, (uint32_t*)(name + 32),
              (uint32_t*)(name + 36), (uint32_t*)(name + 40),
              (uint32_t*)(name + 44));

        const char* p = name;
        while (*p == ' ') {
            p++;
        }

        kprintf("CPU Name : %s\n ", p);
    }
}