/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Tuesday, 3rd November 2020 1:14:33 pm                         *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include <stdio.h>

int ksprintf(char* const str, const char* const format, ...) {
  va_list ap;
  va_start(ap, format);
  const int result = kvsprintf(str, format, ap);
  va_end(ap);
  return result;
}
