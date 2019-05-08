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

#include "MATRIXVoiceOTA.h"
#include <stdio.h>
#include <cmath>
// #include <cstring>

#include "wishbone_bus.h"
#include "everloop.h"
#include "everloop_image.h"
#include "microphone_array.h"
#include "microphone_core.h"
#include "voice_memory_map.h"

#include "kiss_fft.h"
#include "qmath.h"

#define FFT_ORDER 128

//Matrix Voice HAL
namespace hal = matrix_hal;

static hal::WishboneBus wb;
static hal::Everloop everloop;
static hal::EverloopImage image1d;
static hal::MicrophoneArray mics;

// arguments: YOUR_WIFI_SSID, YOUR_WIFI_PASS, YOUR_CHOSEN_ESP32_ID, YOUR_CHOSEN_ESP32_PASSWORD
MATRIXVoiceOTA otaObj("YOUR_WIFI_SSID","YOUR_WIFI_PASS", "YOUR_CHOSEN_ESP32_ID", "YOUR_CHOSEN_ESP32_PASSWORD");

/**
 * Global variables
 */
kiss_fft_cpx cx_in[FFT_ORDER];
kiss_fft_cpx cx_out[FFT_ORDER];
kiss_fft_cfg cfg = kiss_fft_alloc(FFT_ORDER, 0, 0, 0);

/**
 * MAIN SETUP
 */
void setup() {
  // put your setup code here, to run once:
  wb.Init();
  everloop.Setup(&wb);

  mics.Setup(&wb);
  // mics.SetSamplingRate(RATE);

  hal::MicrophoneCore mic_core(mics);
  mic_core.Setup(&wb);


  otaObj.setup();
}


/**
 * MAIN LOOP
 */
void loop() {
  // put your main code here, to run repeatedly:
  otaObj.loop();

  mics.Read();

  for (uint32_t s = 0; s < FFT_ORDER; s++) {
    cx_in[s].r = float2q(fabs(mics.At(s, 0)) / 1024.0);
    cx_in[s].i = 0;
  }

  kiss_fft(cfg, cx_in, cx_out);

  for (uint32_t i = 0; i < image1d.leds.size(); i++) {
    float z = q2double(qmul((cx_out[i + 1].r), (cx_out[i + 1].r)) +
                       qmul((cx_out[i + 1].i), (cx_out[i + 1].i))) *
              256;
    if (i < 6) {
      image1d.leds[i].Set(0, 0, z, 0);
    } else if (i < 12) {
      image1d.leds[i].Set(0, z, 0, 0);
    } else {
      image1d.leds[i].Set(z, 0, 0, 0);
    }
  }
  everloop.Write(&image1d);
}
