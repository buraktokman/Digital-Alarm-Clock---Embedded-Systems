# Digital Alarm Clock (Embedded-Systems Simulator) [![GitHub stars](https://img.shields.io/github/stars/badges/shields.svg?style=social&label=Stars)](https://github.com/buraktokman/Digital-Alarm-Clock---Embedded-Systems/)

[![Travis](https://img.shields.io/travis/rust-lang/rust.svg)](https://github.com/buraktokman/Digital-Alarm-Clock---Embedded-Systems)
[![Repo](https://img.shields.io/badge/source-GitHub-303030.svg?maxAge=3600&style=flat-square)](https://github.com/buraktokman/Digital-Alarm-Clock---Embedded-Systems)
[![Requires.io](https://img.shields.io/requires/github/celery/celery.svg)](https://requires.io/github/buraktokman/Digital-Alarm-Clock---Embedded-Systems/requirements/?branch=master)
[![Scrutinizer](https://img.shields.io/scrutinizer/g/filp/whoops.svg)](https://github.com/buraktokman/Digital-Alarm-Clock---Embedded-Systems)
[![DUB](https://img.shields.io/dub/l/vibe-d.svg)](https://choosealicense.com/licenses/mit/)
[![Donate with Bitcoin](https://img.shields.io/badge/Donate-BTC-orange.svg)](https://blockchain.info/address/17dXgYr48j31myKiAhnM5cQx78XBNyeBWM)
[![Donate with Ethereum](https://img.shields.io/badge/Donate-ETH-blue.svg)](https://etherscan.io/address/91dd20538de3b48493dfda212217036257ae5150)


### Introduction
------
In this project, a digital alarm clock with alarm and temperature abilities built using an Arduino Uno R3 microcontroller for the Spring 2021 term of the Embedded Systems Programming course. This project solely built on Tinkercad simulator.


### Functional Requirements
------
- LCD screen showing current time (24 hours or AM/PM), alarm time (with a status symbol), temperature (Celsius or Fahrenheit)
- Make alarm sound effect using piezo buzzer.
- Button 1 and 2 should be hold 3 seconds to enter time/alarm setup.
- During time/alarm setup, selected value must blink.
- Timing operations must be implemented with TIMERS.


### Materialization
------
Components used in the project listed:
- Arduino Uno R3 x1
- Pushbutton x4
- Potentiometer (10K Ohm) x1
- LCD Display (16x2) x1
- Piezo Buzzer x1
- Temperature Sensor [TMP36] x1
- Resistor (220 Ohm) x1
- Resistor (10K Ohm) x4
- Wires x14

Components connected to the Arduino as follows:
**LCD Display:**
- Pin 1 (GND) on the LCD to ground on the Arduino.
- Pin 2 (VCC) on the LCD to 5V on the Arduino.
- Pin 3 (V0) on the LCD to middle pin (wiper) of the Potentiometer.
- Pin 4 (RS) on the LCD to digital pin 12 of the Arduino.
- Pin 5 (RW) on the LCD to ground on the Arduino.
- Pin 6 (E) ontheLCD to digital pin 11 of the Arduino.
- Pin (DB4 – DB7) on the LCD to the digital pins 6, 5, 4, 3 of the Arduino.
- Pin 15 (LED) on the LCD to 5V pin on the Arduino.
- Pin 16 (LED) on the LCD to ground on the Arduino through the 200-ohm resistor.

**Temperature Sensor:** Vout of the temperature sensor to analog pin A0 on the Arduino.
**PiezoBuzzer:** Positive pin of the piezo buzzer to digital pin 2 on the Arduino.
**Pushbuttons:**
- Button 1 to digital pin 10 on the Arduino through a 10K-ohm resistor.
- Button 2 to digital pin 9 on the Arduino through a 10K-ohm resistor.
- Button 3 to digital pin 8 on the Arduino through a 10K-ohm resistor.
- Button 4 to digital pin 7 on the Arduino through a 10K-ohm resistor.


### Code Design
------
First and foremost, LiquidCrystal.h library included, digital and analog pins that are used in the circuit assigned to variables accordingly. Before the setup function, the global variables
initiated.

**Setup*
In the setup( ) function, pin modes defined, LCD and serial connection begin. All interrupts disabled before the timer configuration and enabled after the configuration.

**Timer**
In this project Timer1 with prescaler 256 used for the timing and compare match interrupt enabled. Compare match register (OCR1A) top value set to 62,500.
Timer1 reaches from 0 to value 62,500 in a second (1,000ms) and triggers the interrupt. In the ISR (Interrupt Service Routine), timer value TCNT1 reset to 0 then the updateClock( ) function executed which adds +1 second to the clock.

Loop( )** function checks the screenMode variable and shows the active screen. The default mode is 0 which executes the clock( ) function to show the live clock, temperature, and alarm status. Then checks if it’s time to alert if the clock equals to the alarm clock.
At the end checkButton( ) function called in order to check if a button pressed or not.
**Change Hour Mode:** Press B4 to switch between 12-hour AM/PM) and 24-hour modes.
**Change Temperature Mode:** Press B3 to switch between 12-hour AM/PM) and 24-hour modes.

**Set Time:**
1. Hold B1 for 3 seconds to enter the setup.
2. Press B2 to switch between the hour and minute.
3. Press B3 to increase the value (Selected value will blink)
4. Press B1 to exit the setup.

**Set Alarm:**
1. Hold B2 for 3 seconds to enter the setup.
2. Press B1 to switch between the hour and minute.
3. Press B3 to increase the value (Selected value will blink)
4. Press B2 to exit the setup


### LICENSE
------

EULA License
