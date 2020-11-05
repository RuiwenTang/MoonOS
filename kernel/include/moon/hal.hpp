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