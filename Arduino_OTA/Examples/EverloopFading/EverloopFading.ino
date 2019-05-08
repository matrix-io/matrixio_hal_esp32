#include "MATRIXVoiceOTA.h"

#include "wishbone_bus.h"
#include "everloop.h"
#include "everloop_image.h"
#include "voice_memory_map.h"

//Matrix Voice
namespace hal = matrix_hal;
static hal::WishboneBus wb;
static hal::Everloop everloop;
static hal::EverloopImage image1d;

unsigned counter = 0;

// arguments: YOUR_WIFI_SSID, YOUR_WIFI_PASS, YOUR_CHOSEN_ESP32_ID, YOUR_CHOSEN_ESP32_PASSWORD
MATRIXVoiceOTA otaObj("YOUR_WIFI_SSID","YOUR_WIFI_PASS", "YOUR_CHOSEN_ESP32_ID", "YOUR_CHOSEN_ESP32_PASSWORD");

void setup() {
  // put your setup code here, to run once:
  wb.Init();
  everloop.Setup(&wb);
  otaObj.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  otaObj.loop();

  for (hal::LedValue& led : image1d.leds) {
    led.red = 0;
    led.green = 0;
    led.blue = static_cast<int>(std::sin(counter / 64.0) * 10.0) + 10;
    led.white = 0;
  }

  everloop.Write(&image1d);
  ++counter;
  delay(5);
}
