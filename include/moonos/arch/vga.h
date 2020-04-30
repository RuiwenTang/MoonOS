#ifndef __MOONOS_VGA_H__
#define __MOONOS_VGA_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif  // __MOONOS_VGA_H__