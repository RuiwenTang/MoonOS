#include <moon/memory.h>
#include <string.h>

#include <moon/tty.hpp>

enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

/**
 * @brief composite vga text color
 *  0000 | 0000
 *   bg     fg
 * @param fg front color
 * @param bg back color
 * @return uint8_t
 */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return (uint8_t)(fg | bg << 4);
}

/**
 * @brief composite vga text raw data
 *  00000000 | 00000000
 *   color       char
 *
 * @param uc
 * @param color
 * @return uint16_t
 */
static inline uint16_t vga_entry(uint8_t uc, uint8_t color) {
  return (uint16_t)uc | ((uint16_t)color << 8);
}

static TTY gTTY;

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* VGA_MEMORY = (uint16_t*)VA(0xB8000);

TTY* TTY::Instance() { return &gTTY; }

TTY::TTY()
    : fRow(0),
      fColumn(0),
      fColor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK)),
      fBuffer(VGA_MEMORY) {}

void TTY::Init() {
  fRow = 0;
  fColumn = 0;
  fColor = vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
  fBuffer = VGA_MEMORY;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      size_t index = y * VGA_WIDTH + x;
      this->fBuffer[index] = vga_entry(' ', fColor);
    }
  }
}

void TTY::PutEntry(uint8_t c, uint8_t color, size_t x, size_t y) {
  size_t index = y * VGA_WIDTH + x;
  fBuffer[index] = vga_entry(c, color);
}

void TTY::PutChar(char c) {
  this->PutEntry(c, fColor, fColumn, fRow);
  if (++fColumn == VGA_WIDTH) {
    fColumn = 0;
    if (++fRow == VGA_HEIGHT) {
      fRow = 0;
    }
  }
}

void TTY::Write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (data[i] == '\n') {
      fRow += 1;
      fColumn = 0;
      continue;
    }
    this->PutChar(data[i]);
  }
}

void TTY::WriteString(const char* data) { this->Write(data, strlen(data)); }
