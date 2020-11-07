/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 4th November 2020 2:00:58 pm                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#pragma once

#include <stdint.h>

namespace IO {

void Write8(uint16_t port, uint8_t data);

uint8_t Read8(uint16_t port);

void Write16(uint16_t port, uint16_t data);

uint16_t Read16(uint16_t port);

void Write32(uint16_t port, uint32_t data);

uint32_t Read32(uint16_t port);

}  // namespace IO