#ifndef __IOPORT_H__
#define __IOPORT_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void out8(unsigned short port, unsigned char data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "d"(port));
}

static inline unsigned char in8(unsigned short port) {
    unsigned char value;

    __asm__ volatile("inb %1, %0" : "=a"(value) : "d"(port));
    return value;
}

static inline void out16(unsigned short port, unsigned short data) {
    __asm__ volatile("outw %0, %1" : : "a"(data), "d"(port));
}

static inline unsigned short in16(unsigned short port) {
    unsigned short value;

    __asm__ volatile("inw %1, %0" : "=a"(value) : "d"(port));
    return value;
}

static inline void out32(unsigned short port, unsigned int data) {
    __asm__ volatile("outl %0, %1" : : "a"(data), "d"(port));
}

static inline unsigned int in32(unsigned short port) {
    unsigned int value;

    __asm__ volatile("inl %1, %0" : "=a"(value) : "d"(port));
    return value;
}

#ifdef __cplusplus
}
#endif

#endif /*__IOPORT_H__*/
