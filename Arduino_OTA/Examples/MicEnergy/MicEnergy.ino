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
#include <cstring>

#include "wishbone_bus.h"
#include "everloop.h"
#include "everloop_image.h"
#include "microphone_array.h"
#include "microphone_core.h"
#include "voice_memory_map.h"

#define RATE 16000

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

int counter = 0;
uint64_t instantE = 0;
uint64_t avgEnergy = 0;
std::valarray<uint64_t> localAverage(20);

/**
 * MAIN SETUP
 */
void setup() {
  // put your setup code here, to run once:
  wb.Init();
  everloop.Setup(&wb);

  mics.Setup(&wb);
  mics.SetSamplingRate(RATE);

  hal::MicrophoneCore mic_core(mics);
  mic_core.Setup(&wb);

  localAverage = 0;

  otaObj.setup();
}


/**
 * MAIN LOOP
 */
void loop() {
  // put your main code here, to run repeatedly:
  otaObj.loop();

  mics.Read(); /* Reading 8-mics buffer from de FPGA */
  instantE = 0;
  for (uint32_t s = 0; s < mics.NumberOfSamples(); s++) {
    instantE = instantE + (mics.At(s, 0)) * (mics.At(s, 0));
  }

  localAverage[counter % 20] = instantE;
  avgEnergy = 0;
  for (auto& data : localAverage) {
    avgEnergy = (avgEnergy + data);
  }

  avgEnergy = avgEnergy / 20;

  for (auto& led : image1d.leds) {
    led.blue = avgEnergy >> 24;
  }
  everloop.Write(&image1d);

  counter++;
}
