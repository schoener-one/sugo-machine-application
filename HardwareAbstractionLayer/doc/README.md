# Hardware Abstraction Layer Documentation

This document describes the hardware abstraction layer (HAL) devices and their architectural abstraction.

# Hardware devices

## Main board

The main CPU board is connected to all further devices which are controlled
by the firmware. The **HAL** supports the following boards.

1. [Raspberry Pi 3 B+](Main_Board/RaspberryPi_3B+/RPI_B3+_Pinout.png)
2. [Raspberry Pi CM3](Main_Board/RaspberryPi_CM3+/RPI_CM3.pdf)

The board is powered by a 5V USB connector.

### Interfaces

The main board supports the following interfaces.

| Interface | Periphery devices               |
|-----------|---------------------------------|
| Ethernet  | -                               |
| I2C       | stepper motor controller boards (1/2) |
| SPI       | SPI RTD temperature sensor board |
| GPIO      | relay board, signal input board, SPI RTD temperature sensor board control |
| USB       | -                               |
| UART      | control host                    |
|||

The I2C, SPI and UART interfaces are provided over the GPIO pin header.

### GPIO pin header

| Pin | Name      | Connection                          | \| | Connection                       |   Name | Pin |
|:----|:----------|:------------------------------------|:--:|---------------------------------:|-------:|----:|
| 01  | 3.3V DC   | motor-control-dc                    | \| | signal-board-dc                  | 5V DC   | 02 |
| 03  | SDA1 (I2C)| motor-control-i2c-sda               | \| | relay-board-dc                   | 5V DC   | 04 |
| 05  | SCL1 (I2C)| motor-control-i2c-scl               | \| | serial-com-gnd                   | GND     | 06 |
| 07  | GPIO 04   | relay-switch-heater-merger (out)    | \| | serial-com-txd                   | TXD     | 08 |
| 09  | GND       | signal-board-gnd                    | \| | serial-com-rxd                   | RXD     | 10 |
| 11  | GPIO 17   | relay-switch-light-run (out)        | \| | relay-switch-light-ready (out)   | GPIO 18 | 12 |
| 13  | GPIO 27   | temperature-sensor-control-cs-feeder (out) | \| | relay-switch-board-gnd    | GND     | 14 |
| 15  | GPIO 22   | temperature-sensor-control-cs-merger (out) | \| |                           | GPIO 23 | 16 |
| 17  | 3.3V DC   | temperature-sensor-control-dc       | \| | relay-switch-light-power (out)   | GPIO 24 | 18 |
| 19  | MOSI (SPI)| temperature-sensor-control-spi-sdi  | \| |                                  | GND     | 20 |
| 21  | MISO (SPI)| temperature-sensor-control-spi-sdo  | \| | _reserved-for-enc28J60_ (in)     | GPIO 25 | 22 |
| 23  | CLK (SPI) | temperature-sensor-control-spi-clk  | \| |                                  | GPIO 08 | 24 |
| 25  | GND       | temperature-sensor-control-gnd      | \| |                                  | GPIO 07 | 26 |
| 27  | ID_SD     |                                     | \| |                                  | ID_SC   | 28 |
| 29  | GPIO 05   | signal-button-start (in)            | \| |                                  | GND     | 30 |
| 31  | GPIO 06   | signal-button-stop (in)             | \| | signal-filament-tension-low (in) | GPIO 12 | 32 |
| 33  | GPIO 13   | motor-control-error (in)            | \| |                                  | GND     | 34 |
| 35  | GPIO 19   | motor-control-reset (out)           | \| | signal-filament-tension-high (in)| GPIO 16 | 36 |
| 37  | GPIO 26   | relay-switch-fan-feeder (out)       | \| | relay-switch-fan-merger (out)    | GPIO 20 | 38 |
| 39  | GND       | motor-control-gnd                   | \| | relay-switch-heater-feeder (out) | GPIO 21 | 40 |
|||||||

### UART console interface

The UART interface can be used to directly connect with a USB _FTDI_ adapter 
to a console of the device for debugging reason.

**FTTI USB cable:**
| Pin            | Color |
|----------------|-------|
| serial-com-txd | white |
| serial-com-rxd | green |
| -              | red   |
| serial-com-gnd | black |
|||

---
## Relay switch board

The relay switch board is a 8 (2 x 4) channel MOSFET board, which is used to switch the power
of different devices like heater and signal lights.

**Properties:**

* Board power supply: 5V
* 4-Channel MOS FET PLC Amplifier Board Driver Module PWM 3-20V to 3.7-27V DC 10A Optocoupler Isolation
* Strong anti-interference ability, stable performance, input signal and output isolation.
* Input signal: PWM signal between 3~20V
* Output capacity: DC 3.7V~27V, current within 10A per channel
* PWM frequency: 0~200Hz
* Indicator light: with input signal and output indicator light
* Size: 56 x 50 x 10mm

**Connections**

* DC+: The positive pole of the device's DC power supply
* DC-: the negative pole of the device's DC power supply
* PWM: signal input terminal (connected to MCU IO port, PLC interface, DC power supply, etc.)
* GND: signal negative terminal
* OUT+: Positive output terminal (connected to the positive pole of the device)
* OUT-: The negative pole of the output terminal (connect to the negative pole of the device)

**Channel occupancy:**

*Board 1 (12V)*
| Channel | Device           | Device type     | Voltage (out) | Power (out) | Color  |
|---------|------------------|-----------------|---------------|-------------|--------|
| GND     |                  |                 | GND           |             | black  |
| 1       | fan-feeder       | fan             | 12V           | ?W          | white  |
| 2       | fan-merger       | fan             | 12V           | ?W          | gray   |
| 3       | heater-merger    | filament heater | 12V           | 40W         | violet |
| 4       | heater-feeder    | filament heater | 12V           | 40W         | yellow |
|||||||

*Board 2 (5V)*
| Channel | Device           | Device type     | Voltage (out) | Power (out) | Color  |
|---------|------------------|-----------------|---------------|-------------|--------|
| GND     |                  |                 | GND           |             | black (Board 1) |
| 1       | light-power      | LED light       | 5V            | -           | green  |
| 2       | light-run        | LED light       | 5V            | -           | yellow |
| 3       | light-ready      | LED light       | 5V            | -           | orange |
| 4       | _unused_         |                 |               |             | red    |
|||||||

### Filament heater

The heater is used to heat the filament up to ~200°C for melting. To control the temperature the heater element
contains a [NTC-3950 100K temperature sensor](Hot_End_Heater/NTC-3950-100K.pdf). The temperature resistance values
are listed in a [CSV list](Hot_End_Heater/NTC100K_3950.csv) file.

![Heater hot end](images/Long-Distance-J-Head-V6-Hot-End.jpg)

---
## Signal input board

The signal input board is 8 channel input board based on [LTV846 opto-couplers](signal_input_board/LTV_8X6_series.pdf), which protect the
main board from over voltage damages.

**Properties:**

* Board power supply: 5V (out) / 3.3 (in)
* 8 output channels (to main board): 3.3V
* 8 input channels: 5V / max.12V

**Channel occupancy:**

| Channel | Device                | Device type | Color  |
|---------|-----------------------|-------------|--------|
| 0       | button-start          | button      | red    |
| 1       | button-stop           | button      | orange |
| 2       | filament-tension-low  |             | yellow |
| 3       | filament-tension-high |             | green  |
| 4       | _unused_              |             | blue   |
| 5       | _unused_              |             | violet |
| 6       | _unused_              |             | gray   |
| 7       | _unused_              |             | white  |
| GND     | gnd                   |             | brown  |
| DC      | power-supply 5V       |             | black  |
|||||

### Button

Simple press button device for interaction with the user.

---
## SPI RTD temperature sensor board

The [SPI RTD temperature sensor board](Temperature_Sensor/MAX31865_RTD_Board.pdf) contains a [MAX31865](Temperature_Sensor/MAX31865.pdf) chip
and is used to measure temperatures on a [PT100 sensor](Temperature_Sensor/ETP-RT-4-24-PT100B.pdf). 

**Properties:**

* Board power supply: 3-5V (RPI 5V out)
* Sensor types: PT100 (2/3/4 wire)
* Communication: SPI

**PINs:**

| PIN     | Connection              | Color  |
|---------|-------------------------|--------|
| 0       | power-supply 3.3V (VIN) | white  |
| 1       | gnd                     | gray   |
| 2       | _unused_                | -      |
| 3       | spi-clk                 | violet |
| 4       | spi-sdo                 | blue   |
| 5       | spi-sdi                 | green  |
| 6       | cs                      | yellow |
| 7       | _unused_                | white  |
|||||

---
## Stepper motor control board

The stepper motor control board is a [Pololu Tic T249](Pololu_Tic_T249/UserGuideTic.pdf) based on 
a [TB67S249FTG](Pololu_Tic_T249/TB67S249FTG_datasheet_en.pdf) chip board.

### Stepper motor

The [stepper motor](Stepper_Motor/StepperMotor_17HS15-1504S-X1.pdf) is used to feed or coil the filament.

**Properties**:
Type: Nema 17
Amps: 1.5A (per phase)
Volt: 12V

---
## Power supply