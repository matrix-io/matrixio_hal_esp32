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

// arguments: YOUR_WIFI_SSID, YOUR_WIFI_PASS, YOUR_CHOSEN_ESP32_ID, YOUR_CHOSEN_ESP32_PASSWORD
MATRIXVoiceOTA otaObj("YOUR_WIFI_SSID","YOUR_WIFI_PASS", "YOUR_CHOSEN_ESP32_ID", "YOUR_CHOSEN_ESP32_PASSWORD");

/**
 * Global variables
 */

// Variables used for sine wave rainbow logic
int ledCount = image1d.leds.size();
float counter = 0;
const float freq = 0.375;

/**
 * MAIN SETUP
 */
void setup() {
  // put your setup code here, to run once:
  wb.Init();
  everloop.Setup(&wb);
  otaObj.setup();
}


/**
 * MAIN LOOP
 */
void loop() {
  // put your main code here, to run repeatedly:
  otaObj.loop();

  for (int i = 0; i <= 250; i++) {
    // For each led in image1d.leds, set led value
    for (matrix_hal::LedValue &led : image1d.leds) {
      // Sine waves 120 degrees out of phase for rainbow
      led.red =
          (std::sin(freq * counter + (M_PI / 180 * 240)) * 155 + 100) / 10;
      led.green =
          (std::sin(freq * counter + (M_PI / 180 * 120)) * 155 + 100) / 10;
      led.blue = (std::sin(freq * counter + 0) * 155 + 100) / 10;
      // If MATRIX Voice, increment by 1.01
      counter = counter + 1.01;
    }

    // Updates the LEDs
    everloop.Write(&image1d);

    delay(40);
  }

  // Updates the Everloop on the MATRIX device
  everloop.Write(&image1d);

  // For each led in image1d.leds, set led value to 0
  for (matrix_hal::LedValue &led : image1d.leds) {
    // Turn off Everloop
    led.red = 0;
    led.green = 0;
    led.blue = 0;
    led.white = 0;
  }

  // Updates the Everloop on the MATRIX device
  everloop.Write(&image1d);
}
