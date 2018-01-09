# MATRIX Voice HAL for ESP32
This is the oficial set of ESP-IDF components for [MATRIX Voice] (https://www.matrix.one/products/voice)

## ESP-IDF Setup (desktop/laptop side)  
### ESP32 toolchain 

Instructions to set up the ESP32 toolchain:

[https://esp-idf.readthedocs.io/en/latest/get-started/linux-setup.html](https://esp-idf.readthedocs.io/en/latest/get-started/linux-setup.html)

### ESP-IDF development framework

See setup guides for detailed instructions to set up the ESP-IDF:

[https://github.com/espressif/esp-idf](https://github.com/espressif/esp-idf)

It is recommended to set the PATH and IDF_PATH environment variables in the ~/.bashrc file

```
export PATH="$PATH:<route to toolchain>/xtensa-esp32-elf/bin"
export IDF_PATH=<route to ESP-IDF>/esp-idf
```

## ESP32 
Clone de repo MATRIX Voice HAL

```
git clone https://github.com/matrix-io/matrixio_hal_esp32.git
```

Everloop example

```
cd matrixio_hal_esp32/examples/arc_demo
make
```
