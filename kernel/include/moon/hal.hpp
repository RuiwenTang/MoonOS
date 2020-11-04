#pragma once

#include <multiboot/multiboot.h>

class HAL final {
 public:
  HAL() = delete;
  ~HAL() = delete;

  static void Init(multiboot_info_t* mb_info);

 private:
  static void InitCore(multiboot_info_t* mb_info);
  

};