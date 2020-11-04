#include "kprintf.hpp"

#include <stdarg.h>

#include <moon/tty.hpp>
#include <stdio.h>

void kprintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  char buffer[100];
  int length = kvsprintf(buffer, format, ap);
  va_end(ap);
  TTY::Instance()->WriteString(buffer);
}