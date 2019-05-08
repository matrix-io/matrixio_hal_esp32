#ifndef MATRIX_Voice_OTA_H
#define MATRIX_Voice_OTA_H


class MATRIXVoiceOTA {
    public:
        MATRIXVoiceOTA(const char* SSID, const char* PASS, const char* ESP_ID, const char* ESP_PASS);
        void setup();
        void loop();
        void setBaud(int baud);
    private:
        const char* _SSID;
        const char* _PASS;
        const char* _ESP_ID;
        const char* _ESP_PASS;
        int _baud;
};
#endif
