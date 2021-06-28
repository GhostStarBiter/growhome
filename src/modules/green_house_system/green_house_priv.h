#ifndef GROW_GREEN_PRIV_H
#define GROW_GREEN_PRIV_H

#include <stdlib.h>

#include "stm32f10x.h"

// FreeRTOS includes
#include "task.h"
#include "timers.h"

#include "main/application_map.h"

// MCU includes
#include "mcu_peripherals/adc/mcu_adc.h"
#include "mcu_peripherals/gpio/mcu_gpio.h"
#include "mcu_peripherals/uart/mcu_uart.h"

// MODULES includes
#include "water/water.h"
#include "triac/triac.h"
#include "servo/servo.h"
#include "onewire/onewire.h"
#include "schedule/schedule.h"
#include "filter/filter.h"
#include "pi_ctrl/pi_ctrl.h"

// this module
#include "green_house.h"

typedef enum {
  NONE,
  HEAT,
  COOLDOWN,
  IN_RANGE
} temp_control_state_t;

typedef struct {
  FunctionalState status;                  // ON/OF
  uint16_t        duty_ms;
  uint16_t        cycle_counter_ms;
} heater_t;


typedef struct {
  temp_control_state_t  state;
  uint8_t               set_point;
  int8_t                delta;                     // [-127 .. 127]
  pi_ctrl_object_t      pi_controler;
} temperature_t;


typedef struct{
    control_mode_t    mode;                           // MANUAL/AUTOMATIC
    heater_t          heater;
    temperature_t     temperature;

    filter_object_t   water_level;                    // 0 .. 100 %
    filter_object_t   mixed_air_temp;
    filter_object_t   income_air_temp;
    filter_object_t   humidity;

    FunctionalState   light_status;                   // ON/OFF
    FunctionalState   water_pump_status;              // ON/OFF
    FunctionalState   air_income_status;              // ON/OFF
    FunctionalState   air_outlet_valve_status;        // OPEN/CLOSED
    FunctionalState   air_mix_status;                 // ON/OFF

    uint16_t          manual_mode_timeout;
    uint32_t          air_mix_time_counter;

    uint8_t           desired_humidity;
    int8_t            delta_humidity;                 // [-127 .. 127]

} green_house_t;


//--------------------------------------------------------------------------------------------------
/// @brief Update measurements of temperatures, humidity, water level
//--------------------------------------------------------------------------------------------------
static void growbox_update_measurements(void);


//--------------------------------------------------------------------------------------------------
/// @brief Update system run-time data (temp deviation, heater work-time etc.)
//--------------------------------------------------------------------------------------------------
static void growbox_update_statistics(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Growbox parameters (temp, water, light) control function
//--------------------------------------------------------------------------------------------------
static void growbox_control(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Temperature control function
//--------------------------------------------------------------------------------------------------
static void growbox_control_temperature(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Water supply control function
//--------------------------------------------------------------------------------------------------
static void growbox_control_water_supply(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Light control function
//--------------------------------------------------------------------------------------------------
static void growbox_control_light(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Set Income air ventilator intensity
/// @param  Ventilator power to set in percents
//--------------------------------------------------------------------------------------------------
static void growbox_set_income_air_intensity
(
    uint8_t intensity_percents
);


//--------------------------------------------------------------------------------------------------
/// @brief  Set status of Air Mixing ventilator
/// @param  Air Mixing ventilator status (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
static void growbox_set_air_mixing_status
(
    FunctionalState e_air_mixing_status
);


//--------------------------------------------------------------------------------------------------
/// @brief  Set status of Air Heater
/// @param  Air Heater status (ENABLE/DISABLE)
//--------------------------------------------------------------------------------------------------
static void growbox_set_heater_status
(
    FunctionalState heater_state
);


//--------------------------------------------------------------------------------------------------
/// @brief  Run timeout handling for air mix ventilator control
//--------------------------------------------------------------------------------------------------
static void growbox_control_air_mix(bool temp_request);


#endif  // GROW_GREEN_PRIV_H

//  *** END OF FILE ***
