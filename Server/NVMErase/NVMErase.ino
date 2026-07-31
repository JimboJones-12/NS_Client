#include "nvs_flash.h"

void setup() {
  nvs_flash_erase();
  nvs_flash_init();
}

void loop() {
  // Main code
}
