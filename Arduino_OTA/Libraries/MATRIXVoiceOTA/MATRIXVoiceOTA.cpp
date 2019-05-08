#include <MATRIXVoiceOTA.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "esp_system.h"

MATRIXVoiceOTA::MATRIXVoiceOTA(const char* SSID, const char* PASS, const char* ESP_ID, const char* ESP_PASS){
    _SSID = SSID;
    _PASS = PASS;
    _ESP_ID = ESP_ID;
    _ESP_PASS = ESP_PASS;
    _baud = 460800;
}

void MATRIXVoiceOTA::setup() {
    // everloop green
    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(_SSID, _PASS);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        // everloop red
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    //everloop blue
    ArduinoOTA.setHostname(_ESP_ID);
    ArduinoOTA.setPassword(_ESP_PASS);
    ArduinoOTA
        .onStart([]() {
        })
        .onEnd([]() {
        Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

    ArduinoOTA.begin();

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void MATRIXVoiceOTA::loop() {
    ArduinoOTA.handle();
    //delay(1000);
}

void MATRIXVoiceOTA::setBaud(int baud) {
    _baud = baud;
}