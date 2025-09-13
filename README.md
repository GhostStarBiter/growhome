# GreenHouse Project - Automated Embedded System

## Overview

This project is an **Automated Greenhouse System** designed for
sustainable indoor plant growth.\
It combines hardware, embedded software, and IoT connectivity to monitor
and control environmental conditions.

## Features

-   Automated light, temperature, and watering control.
-   Temperature & humidity monitoring.
-   Artificial grow lights with configurable schedules.
-   Irrigation & ventilation system.
-   Real-time monitoring via LCD and WiFi (ESP8266, WIP).
-   Data collection: temperature, humidity, water consumption, air
    exchange.

## Hardware

-   **Controllers:** STM32F103C8T6 + ESP8266 (WiFi, TBD).
-   **Sensors:** GY21 (I2C), OneWire temperature/humidity, analog water
    tank level sensor.
-   **Actuators:** LED grow lights, heater, water pump, fans (air income
    & mixing).
-   **User Interface:** LCD 2x16, rotary encoder with push-button,
    (planned web dashboard).
-   **Design Tools:** KiCad for schematics and PCB.

## Software

-   **IDE:** AC6 STM32 SystemWorkbench (Eclipse-based).
-   **Libraries:** STM32F10x SPL, FreeRTOS.
-   **Peripherals:** ADC+DMA, PWM timers, I2C+DMA, EXTI (encoder), UART,
    SPI.
-   **Tasks:**
    -   `service_task`: System heartbeat & cycle counter.
    -   `growbox_control_rtos_task`: Sensor reading,
        automatic/manual/remote control of lights, fans, heater, water
        pump.
    -   `system_user_interface_task`: LCD display, encoder interaction.
    -   `network_communication_task`: WiFi connectivity (ESP8266, TBD).

## Results

-   Successfully controls temperature, humidity, lighting, and watering
    cycles.
-   Reliable operation with modular tasks.
-   IoT features under development (remote monitoring & control).

## Future Work

-   Complete ESP8266 WiFi integration for IoT dashboard.
-   Add plant-specific grow plans with adaptive temperature control.
-   Expand to multi-node system for larger greenhouses.

## Author

**Dmytro Zelenets**
