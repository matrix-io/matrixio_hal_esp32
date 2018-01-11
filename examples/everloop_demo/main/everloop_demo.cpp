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

int cpp_loop() {
  hal::WishboneBus wb;

  wb.Init();

  hal::Everloop everloop;
  hal::EverloopImage image1d;

  everloop.Setup(&wb);

  unsigned counter = 0;

  while (1) {
    for (hal::LedValue& led : image1d.leds) {
      led.red = 0;
      led.green = 0;
      led.blue = static_cast<int>(std::sin(counter / 64.0) * 10.0) + 10;
      led.white = 0;
    }

    everloop.Write(&image1d);
    ++counter;
  }

  return 0;
}

extern "C" {
void app_main(void) { cpp_loop(); }
}
