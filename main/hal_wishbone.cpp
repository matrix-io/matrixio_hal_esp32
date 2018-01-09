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

#include <cstring>

#include "wishbone_bus.h"

#include "everloop.h"
#include "everloop_image.h"
#include "microphone_array.h"
#include "voice_memory_map.h"

namespace hal = matrix_hal;

struct fpga_version {
  uint32_t identify;
  uint32_t version;
};

void cpp_loop() {
  matrix_hal::WishboneBus wb;

  wb.Init();

  hal::Everloop everloop;
  everloop.Setup(&wb);

  hal::MicrophoneArray mics;
  mics.Setup(&wb);

  hal::EverloopImage image1d;

  fpga_version v;
  wb.SpiRead(hal::kConfBaseAddress, (uint8_t*)&v, sizeof(fpga_version));

  printf("identify = %X\n", v.identify);
  printf("version = %X\n", v.version);
  fflush(stdout);

  int j = 0;
  uint64_t instantE = 0;
  uint64_t avgEnergy = 0;
  std::valarray<uint64_t> localAverage(20);
  localAverage = 0;

  while (true) {
    mics.Read(); /* Reading 8-mics buffer from de FPGA */
    instantE = 0;
    for (uint32_t s = 0; s < mics.NumberOfSamples(); s++) {
      instantE = instantE + (mics.At(s, 0)) * (mics.At(s, 0));
    }

    localAverage[j % 20] = instantE;
    avgEnergy = 0;
    for (auto& data : localAverage) {
      avgEnergy = (avgEnergy + data);
    }

    avgEnergy = avgEnergy / 20;

    for (auto& led : image1d.leds) {
      led.red = avgEnergy >> 24;
    }
    everloop.Write(&image1d);

    j++;
  }
}

extern "C" {
void app_main(void) { cpp_loop(); }
}
