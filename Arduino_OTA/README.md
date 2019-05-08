This guide has been adapted re-worked from https://github.com/Romkabouter/Matrix-Voice-ESP32-MQTT-Audio-Streamer.

# Matrix Voice ESP32 OTA w/ Arduino

## Features

- Runs standalone without the Raspberry Pi after flashing this program
- OTA Updating with set password

## Get started

To get the code running I suggest you first reset the Voice if you have flashed it previously

- Follow Step 1 and 2 from this guide https://matrix-io.github.io/matrix-documentation/matrix-voice/esp32/
- ssh into the pi, execute this command: voice_esp32_enable. If you get a permission denied, execute the command again. 
- Reset memory: esptool.py --chip esp32 --port /dev/ttyS0 --baud 115200 --before default_reset --after hard_reset erase_flash
- Reboot the Pi.

## Install & Setup Arduino IDE w/ ESP32
Follow the directions on the Espressif github: https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md

## OTA (Over the Air) Update version

In the folder "starter", there are two bin files:
- bootloader.bin
- partitions_two_ota.bin
These files are needed in order to do the first flashing. You can build your own versions of it by checking out the OTABuilder folder.
In there is a program in c++, which does nothing but if you do a make menuconfig you will see that the partition is set to OTA.
When you do a make, the partitions_two_ota.bin will be in the build folder and the bootloader.bin in the build/bootloader folder.

To flash the OTA version for the first time, attach the Voice to a Raspberry Pi. 
- Get the HAL code from https://github.com/matrix-io/matrixio_hal_esp32/tree/master/components/hal
- Copy the folder "hal" to your Arduino IDE libraries folder
- Clone this repo on your PC
- Copy the folder "MATRIXVoiceOTA" to your Arduino IDE libraries folder
- Open the starter.ino file in the Arduino IDE
- Select ESP32 Dev Module as Board, set flash size to 4MB and Upload speed to 460800
- Change the ssid, password (for your WiFi), ESP_ID, ESP_PASS (you will be asked to enter password when uploading over OTA) to fit your needs
- Go to "Sketch" -> "Export compiled binary"
- Move the Starter.ino.esp32.bin file into the starter directory with deploy_ota.sh
- In the file deploy_ota.sh, change to IP address to the IP your Pi is on.
- Open a terminal (Linux and Mac should work, tested on Mac), change to the folder where this code is and run from your PC: sh deploy_ota.sh
- You should see the message: "esptool.py wrapper for MATRIX Voice"
- Watch how it flashes and when it restarts, the ledring should turn blue.
- Remove the voice from the Pi and plug the power into the Voice with a micro usb cable, the Voice should start
- Open your Arduino IDE again, after a while the Matrix Voice should show up as a network port ("Tools" -> "Port"), select this port
- Make a change (or not) and do a Sketch -> Upload
- Sometimes uploading fails, especially the first few times. You will be prompted to enter the password the first few times. Once the first upload works, the rest of them are pretty consistent.

If you change code and OTA does not work anymore for some reason, you can always start over by doing the "get started" part from the second step.

## Known issues
- Uploading a sketch sometimes fails or an error is thrown when the uploading is done. If you get the error, check if your new sketch has been implemented or start over.

