#pragma once
#include <stddef.h>
#include <stdint.h>
class TTY final {
 public:
  static TTY* Instance();
  void Init();
  void PutChar(char c);
  void Write(const char* data, size_t size);
  void WriteString(const char* str);
  TTY();
  ~TTY() = default;

 private:
  void PutEntry(uint8_t c, uint8_t color, size_t x, size_t y);

 private:
  size_t fRow;
  size_t fColumn;
  size_t fColor;
  uint16_t* fBuffer;
};