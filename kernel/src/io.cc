#include <moon/io.hpp>

namespace IO {

void Write8(uint16_t port, uint8_t data) {
  __asm__ volatile("outb %0, %1" : : "a"(data), "d"(port));
}

uint8_t Read8(uint16_t port) {
  unsigned char value;

  __asm__ volatile("inb %1, %0" : "=a"(value) : "d"(port));
  return value;
}

void Write16(uint16_t port, uint16_t data) {
  __asm__ volatile("outw %0, %1" : : "a"(data), "d"(port));
}

uint16_t Read16(uint16_t port) {
  unsigned short value;

  __asm__ volatile("inw %1, %0" : "=a"(value) : "d"(port));
  return value;
}

void Write32(uint16_t port, uint32_t data) {
  __asm__ volatile("outl %0, %1" : : "a"(data), "d"(port));
}

uint32_t Read32(uint16_t port) {
  unsigned int value;

  __asm__ volatile("inl %1, %0" : "=a"(value) : "d"(port));
  return value;
}

}  // namespace IO