/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Monday, 2nd November 2020 10:00:49 pm                         *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#pragma once

#include <multiboot/multiboot.h>

class HAL final {
 public:
  static HAL* Instance();

  void Init(multiboot_info_t* mb_info);

 private:
  HAL() = default;
  ~HAL() = default;
  void InitCore(multiboot_info_t* mb_info);
};