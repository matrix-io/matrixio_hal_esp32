#include "MATRIXVoiceOTA.h"

// #include <gflags/gflags.h>
#include <stdint.h>
// #include <string.h>

#include <stdio.h>
#include <cmath>
// #include <cstring>

#include <fstream>
#include <iostream>
#include <string>
#include <valarray>

#include "kiss_fft.h"
#include "qmath.h"

#include "direction_of_arrival.h"

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
static hal::DirectionOfArrival doa(mics);

// arguments: YOUR_WIFI_SSID, YOUR_WIFI_PASS, YOUR_CHOSEN_ESP32_ID, YOUR_CHOSEN_ESP32_PASSWORD
MATRIXVoiceOTA otaObj("YOUR_WIFI_SSID","YOUR_WIFI_PASS", "YOUR_CHOSEN_ESP32_ID", "YOUR_CHOSEN_ESP32_PASSWORD");


/**
 * Global variables
 */
int led_offset[] = {23, 27, 32, 1, 6, 10, 14, 19};
int lut[] = {1, 2, 10, 200, 10, 2, 1};

float azimutal_angle;
float polar_angle;
int mic;


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

  doa.Init();


  otaObj.setup();
}


/**
 * MAIN LOOP
 */
void loop() {
  // put your main code here, to run repeatedly:
  otaObj.loop();

  mics.Read(); /* Reading 8-mics buffer from de FPGA */

  doa.Calculate();

  azimutal_angle = doa.GetAzimutalAngle() * 180 / M_PI;
  polar_angle = doa.GetPolarAngle() * 180 / M_PI;
  mic = doa.GetNearestMicrophone();

  for (hal::LedValue &led : image1d.leds) {
    led.blue = 0;
  }

  for (int i = led_offset[mic] - 3, j = 0; i < led_offset[mic] + 3;
       ++i, ++j) {
    if (i < 0) {
      image1d.leds[image1d.leds.size() + i].blue = lut[j];
    } else {
      image1d.leds[i % image1d.leds.size()].blue = lut[j];
    }

    everloop.Write(&image1d);
  }
}
