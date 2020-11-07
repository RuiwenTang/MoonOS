/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 4th November 2020 1:33:10 pm                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include "kprintf.hpp"

#include <stdarg.h>

#include <moon/tty.hpp>
#include <stdio.h>

void kprintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  char buffer[100];
  kvsprintf(buffer, format, ap);
  va_end(ap);
  TTY::Instance()->WriteString(buffer);
}