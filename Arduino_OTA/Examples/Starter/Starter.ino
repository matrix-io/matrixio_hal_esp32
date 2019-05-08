#include "MATRIXVoiceOTA.h"

#include "wishbone_bus.h"
#include "everloop.h"
#include "everloop_image.h"
#include "microphone_array.h"
#include "microphone_core.h"
#include "voice_memory_map.h"

#define RATE 16000

//Matrix Voice
namespace hal = matrix_hal;
static hal::WishboneBus wb;
static hal::Everloop everloop;
static hal::MicrophoneArray mics;
static hal::EverloopImage image1d;

// arguments: YOUR_WIFI_SSID, YOUR_WIFI_PASS, YOUR_CHOSEN_ESP32_ID, YOUR_CHOSEN_ESP32_PASSWORD
MATRIXVoiceOTA otaObj("YOUR_WIFI_SSID","YOUR_WIFI_PASS", "YOUR_CHOSEN_ESP32_ID", "YOUR_CHOSEN_ESP32_PASSWORD");

void setup() {
  // put your setup code here, to run once:

  wb.Init();
  everloop.Setup(&wb);

  //setup mics
  mics.Setup(&wb);
  mics.SetSamplingRate(RATE);
  //mics.SetGain(5);

   // Microphone Core Init
  hal::MicrophoneCore mic_core(mics);
  mic_core.Setup(&wb);

  otaObj.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  otaObj.loop();

        for (hal::LedValue& led : image1d.leds) {
            led.red = 0;
            led.green = 0;
            led.blue = 0;
            led.white = 5;
    }

    everloop.Write(&image1d);
}
