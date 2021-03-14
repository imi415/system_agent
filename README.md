# My sensor runs Linux!

This is the source repository for a simple system monitor program,

which runs on Linux.

## Supported sensors

- [ ] Bosch SensorTec BME280 digital temperature/humidity/pressure sensor
- [ ] AMS(Sciosense) CCS811 eTVOC sensor
- [ ] MH-Z19 series NDIR eCO2 sensor 
- [ ] TDK InvenSense 6-Axis gyro/accelerometer
- [ ] General GPIO based water detector sensors

## Other supported devices

- [ ] ST7789 IPS TFT LCD(240px * 240px)
- [ ] General GPIO based buttons
- [ ] Genetal GPIO based LEDs

## Libraries used

- [x] LittleVGL
- [ ] mosquitto
- [ ] libgpiod
- [ ] libconfig
- [ ] libpthread
- [ ] libiio

## How to compile

* Native:

```bash
mkdir build && cd build
cmake ..
make -j${nproc}
```

* Cross:
```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE_NAME} .. # Replace to yours
make -j${nproc}
```
