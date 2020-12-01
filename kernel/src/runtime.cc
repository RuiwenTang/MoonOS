/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 4th November 2020 10:03:07 pm                      *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#include <stddef.h>

#include "umm_malloc.h"

namespace __cxxabiv1 {
/* guard variables */

/* The ABI requires a 64-bit type.  */
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release(__guard *);
extern "C" void __cxa_guard_abort(__guard *);

extern "C" int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); }

extern "C" void __cxa_guard_release(__guard *g) { *(char *)g = 1; }

extern "C" void __cxa_guard_abort(__guard *) {}

}  // namespace __cxxabiv1

void *operator new(size_t size) { return umm_malloc(size); }

void *operator new[](size_t size) { return umm_malloc(size); }

void operator delete(void *p) { umm_free(p); }

void operator delete(void *p, size_t) { umm_free(p); }

void operator delete[](void *p) { umm_free(p); }

void operator delete[](void *p, size_t) { ::operator delete[](p); }