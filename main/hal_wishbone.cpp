/*
 * Copyright 2017 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Voice HAL for ESP32
 *
 * Author:
 *       Andrés Calderón <andres.calderon@admobilize.com>
 *
 * MATRIX Voice ESP32 HAL is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <cmath>

#include "esp_system.h"

#include "everloop.h"
#include "everloop_image.h"
#include "voice_memory_map.h"
#include "wishbone_bus.h"

namespace hal = matrix_hal;

struct fpga_version {
  uint32_t identify;
  uint32_t version;
};

void cpp_loop() {
  matrix_hal::WishboneBus wb;

  esp_err_t ret = wb.Init();

  printf("init %d\n", ret);
  fflush(stdout);

  hal::Everloop everloop;
  hal::EverloopImage image1d;

  everloop.Setup(&wb);

  unsigned counter = 0;

  fpga_version v;
  wb.SpiRead(hal::kConfBaseAddress, (uint8_t*)&v, sizeof(fpga_version));

  printf("identify = %d\n", v.identify);
  printf("version = %d\n", v.version);
  fflush(stdout);

  while (0) {
    for (hal::LedValue& led : image1d.leds) {
      led.red = 0;
      led.green = 0;
      led.blue = static_cast<int>(std::sin(counter / 128.0) * 7.0) + 8;
      led.white = 0;
    }

    everloop.Write(&image1d);
    ++counter;

    if (counter == 1024) esp_restart();
  }
  esp_restart();
}

extern "C" {
void app_main(void) { cpp_loop(); }
}
