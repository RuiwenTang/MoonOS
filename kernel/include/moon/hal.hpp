#pragma once

#include <multiboot/multiboot.h>

class HAL final {
 public:
  HAL();
  ~HAL();

  void Init(multiboot_info_t* mb_info);

 private:
  void InitCore(multiboot_info_t* mb_info);
  

};