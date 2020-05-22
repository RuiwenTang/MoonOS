#ifndef __MOONOS_STDARG_H__
#define __MOONOS_STDARG_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef __builtin_va_list va_list;
/// Initialize a variable argument list
#define va_start __builtin_va_start
/// Retrieve next argument
#define va_arg __builtin_va_arg
/// End using variable argument list
#define va_end __builtin_va_end
/// copies the (previously initialized) variable argument list
#define va_copy __builtin_va_copy

#ifdef __cplusplus
}
#endif

#endif  // __MOONOS_STDARG_H__