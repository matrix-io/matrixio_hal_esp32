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

#include "kiss_fft.h"
#include "qmath.h"

namespace hal = matrix_hal;

void cpp_loop() {
  matrix_hal::WishboneBus wb;

  wb.Init();

  hal::Everloop everloop;
  everloop.Setup(&wb);

  hal::MicrophoneArray mics;
  mics.Setup(&wb);

  hal::EverloopImage image1d;

  kiss_fft_cpx cx_in[mics.NumberOfSamples()];
  kiss_fft_cpx cx_out[mics.NumberOfSamples()];
  kiss_fft_cfg cfg = kiss_fft_alloc(mics.NumberOfSamples(), 0, 0, 0);

  while (1) {
    mics.Read();

    for (uint32_t s = 0; s < mics.NumberOfSamples(); s++) {
      cx_in[s].r = float2q(fabs(mics.At(s, 0)) / 1024.0);
      cx_in[s].i = 0;
    }

    kiss_fft(cfg, cx_in, cx_out);

    for (uint32_t i = 0; i < image1d.leds.size(); i++) {
      float z = q2double(qmul((cx_out[i+1].r), (cx_out[i+1].r)) +
                         qmul((cx_out[i+1].i), (cx_out[i+1].i)));
      image1d.leds[i].blue = z * 512;
    }
    everloop.Write(&image1d);
  }
}

extern "C" {
void app_main(void) { cpp_loop(); }
}
