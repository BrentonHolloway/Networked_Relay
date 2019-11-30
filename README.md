# Networked Relay
**Author:** Brenton Holloway <br/>
**Version:** 1.0.0

## Table of Contents
- [Networked Relay](#networked-relay)
  - [Table of Contents](#table-of-contents)
  - [Description](#description)
  - [Development Requirements](#development-requirements)
  - [Compiling resources](#compiling-resources)
  - [Hardware Requirements](#hardware-requirements)
    - [EtherMega specifications](#ethermega-specifications)
      - [Microcontroller](#microcontroller)
      - [EtherMega](#ethermega)
      - [Memory](#memory)
      - [Communications](#communications)
    - [Networked Relay Pinout](#networked-relay-pinout)

## Description
Networked Realy used to turn on and off remote 240v power outlets.

## Development Requirements
- Arduino Compiler
- npm
- gulp

## Compiling resources
To compile resources use gulp.

gulp sass - Compiles sass files into css
gulp js - Compresses js files
gulp html - Compresses html and htm files
gulp watch - continuously watches files for changes and recompiles files upon change

## Hardware Requirements
This project is designed to run on an EtherMega designed by FreeTronics link to the page can be found below. The EtherMega has an Atmel ATmega 2560 MCU.

This project uses am 8-Channel Relay Driver Shield that allows the EtherMega to turn
on and off up to 6 relays with just 2 pins via I2C.

**EtherMega:** https://www.freetronics.com.au/collections/arduino/products/ethermega-arduino-mega-2560-compatible-with-onboard-ethernet <br/>
**8-Channel Relay Driver Shield:** https://www.freetronics.com.au/collections/shields/products/relay8-8-channel-relay-driver-shield <br/>
**Real Time Clock (RTC) Module:** https://www.freetronics.com.au/collections/modules/products/real-time-clock-rtc-module <br/>
**Power Regulator 28V for PoE and Project Powering:** https://www.freetronics.com.au/collections/modules/products/poe-power-regulator-28v <br/>
**LCD & Keypad Shield:** https://www.freetronics.com.au/collections/display/products/lcd-keypad-shield


### EtherMega specifications
#### Microcontroller
|||
|-|-|
| MCU Type | Atmel ATmega 2560 |
| Operating Voltage | 5v |
| MCU Clock Speed | 16 MHz |

#### EtherMega
|||
|-|-|
| Input Voltage | 7 - 28v DC (onboard switchmode power supply) |
| Digital I/O Pins | 54 (14 PWM) |
| Analog Input Pins | 16 (analog input pins also support digital I/O, giving a total of 70 I/O pins if required) |
| Analog Resolution | 10 bits, 0 - 1023 |
| Current Per I/O Pin | 40 mA max |
| Total Current for all I/O pins | 200 mA |
| Current at 3.3v | 50 mA |

#### Memory
|||
|-|-|
| Flash Memory | 256 KB Flash Memory, of which 8 KB is used by the bootloader |
| SRAM, EEPROM | 8 KB SRAM, 4 KB EEPROM |
| micro SD | micro SD card slot sith SPI interface. Uses pins D4 (select), D50, D51, D52 |

#### Communications
|||
|-|-|
| Serial | 4 x hardware USARTs, SPI (Serial Peripheral Interface), I2C |
| Ethernet | 1 x 10/100 LAN port using the Wiznet W5100. Uses pins D10 (select), D50, D51, D52 |
| Other | Integrated USB programming and communication port. Many other one-wire, multi-wire, LCD and expansion devices supported by free code and libraries |

### Networked Relay Pinout

| Pin Number on EtherMega | Connection |
|---------|------------|
| 0       | TX |
| 1       | RX |
| 2       |
| 3       |
| 4       |
| 5       |
| 6       |
| 7       |
| 8       |
| 9       |
| 10      |
| 11      |
| 12      |
| 13      |
| 14      |
| 15      |
| 16      |
| 17      |
| 18      |
| 19      |
| 20      |
| 21      |
| 22      |
| 23      |
| 24      |
| 25      |
| 26      |
| 27      |
| 28      |
| 29      |
| 30      |
| 31      |
| 32      |
| 33      |
| 34      |
| 35      |
| 36      |
| 37      |
| 38      |
| 39      |
| 40      |
| 41      |
| 42      |
| 43      |
| 44      |
| 45      |
| 46      |
| 47      |
| 48      |
| 49      |
| 50      |
| 51      |
| 52      |
| 53      |