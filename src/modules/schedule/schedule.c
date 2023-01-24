#include "configuration/app_config.h"

#include "rtc/mcu_rtc.h"
#include "schedule.h"


#define DAY_DEFAULT_LIGHT_DURATION          GROWBOX_MAX_LIGHT_ON_HOURS*SECONDS_IN_HOUR

typedef struct{
    activity_schedule_t light;
    activity_schedule_t water;
} day_plan_t;

typedef struct{
  mcu_time_t  current_time;
  day_plan_t  of;
} schedule_t;


//------------------------------------------------------------------------------
/// @brief  Check schedule of light
//------------------------------------------------------------------------------
static void check_light_schedule(
    mcu_time_t      current_time,
    FunctionalState *greenhouse_light_state
);


//------------------------------------------------------------------------------
/// @brief  Check schedule of water
//------------------------------------------------------------------------------
static void check_water_schedule(
    mcu_time_t      current_time,
    FunctionalState *greenhouse_water_pump_state
);


volatile schedule_t schedule;

//--------------------------------------------------------------------------------------------------
void schedule_init(void)
{
  mcu_time_t init_time;

  // ***
  init_time.hour  = 6;
  init_time.min   = 59;
  init_time.sec   = 50;
  mcu_rtc_set_time(init_time);

  init_time = mcu_rtc_get_time();

  schedule.current_time.hour  = init_time.hour;
  schedule.current_time.min   = init_time.min;

}


//--------------------------------------------------------------------------------------------------
void schedule_update
(
    FunctionalState *greenhouse_light_state,
    FunctionalState *greenhouse_water_pump_state
)
{
  mcu_time_t current_time = mcu_rtc_get_time();

  check_light_schedule(
      current_time,
      greenhouse_light_state
  );


  check_water_schedule(
      current_time,
      greenhouse_water_pump_state
  );

}


//--------------------------------------------------------------------------------------------------
static void check_light_schedule(
    mcu_time_t      current_time,
    FunctionalState *greenhouse_light_state)
{
  uint32_t minutes_schedule, minutes_current_time;

  minutes_schedule = schedule.of.light.on_time.hour*60 + schedule.of.light.on_time.min;
  minutes_current_time = current_time.hour*60 + current_time.min;

  if( minutes_current_time >= minutes_schedule )
  {
    *greenhouse_light_state = ENABLE;
  }
  else
  {
    *greenhouse_light_state = DISABLE;
  }

  schedule_light_update_expected_endtime();

  minutes_schedule = schedule.of.light.expected_endtime.hour*60 + schedule.of.light.expected_endtime.min;

  if(*greenhouse_light_state == ENABLE &&
      minutes_schedule <= minutes_current_time
     )
  {
    *greenhouse_light_state = DISABLE;
  }
}


//--------------------------------------------------------------------------------------------------
void schedule_light_update_expected_endtime(void)
{
  // convert duration to minutes
  schedule.of.light.expected_endtime.min =
      schedule.of.light.duration_sec % 60;
  // extract duration hours
  schedule.of.light.expected_endtime.hour  =
      schedule.of.light.duration_sec / 3600;

  schedule.of.light.expected_endtime.min   +=
      schedule.of.light.on_time.min;

  schedule.of.light.expected_endtime.hour  +=
      schedule.of.light.on_time.hour;

  if(schedule.of.light.expected_endtime.min > 59)
  {
    schedule.of.light.expected_endtime.min %= 60;
    schedule.of.light.expected_endtime.hour++;
  }

  if(schedule.of.light.expected_endtime.hour > 23)
  {
    schedule.of.light.expected_endtime.hour = 23;
    schedule.of.light.expected_endtime.min = 59;
  }
}


//--------------------------------------------------------------------------------------------------
static void check_water_schedule(
    mcu_time_t      current_time,
    FunctionalState *greenhouse_water_pump_state)
{
  bool new_interval_started;

  uint32_t minutes_current_time, seconds_current_time;

  minutes_current_time = current_time.hour*60 + current_time.min;

  seconds_current_time = minutes_current_time*60 + current_time.sec;

  new_interval_started = seconds_current_time % (schedule.of.water.interval_minutes*60);

  if(!new_interval_started)
  {
    schedule.of.water.expected_endtime.sec = seconds_current_time + schedule.of.water.duration_sec;
    *greenhouse_water_pump_state = ENABLE;
  }

  if(seconds_current_time >= schedule.of.water.expected_endtime.sec)
  {
    *greenhouse_water_pump_state = DISABLE;

  }

}


//--------------------------------------------------------------------------------------------------
result_t schedule_set_light_schedule(
    activity_schedule_t new_light_schedule
)
{
  result_t result = result_fail;

  if( new_light_schedule.on_time.hour < 23 &&
      new_light_schedule.on_time.min < 59 &&
      new_light_schedule.on_time.sec < 59 &&
      new_light_schedule.duration_sec < 24 * SECONDS_IN_HOUR)
  {
    schedule.of.light.on_time       = new_light_schedule.on_time;
    schedule.of.light.duration_sec  = new_light_schedule.duration_sec;
    result = result_success;
  }

  return result;
}


//--------------------------------------------------------------------------------------------------
result_t schedule_set_water_schedule(
    activity_schedule_t new_water_schedule
)
{
  result_t result = result_fail;

  if( new_water_schedule.on_time.hour < 23 &&
      new_water_schedule.on_time.min < 59 &&
      new_water_schedule.on_time.sec < 59 &&
      new_water_schedule.duration_sec < 24 * SECONDS_IN_HOUR &&
      new_water_schedule.interval_minutes != 0)
  {
    schedule.of.water.on_time           = new_water_schedule.on_time;
    schedule.of.water.duration_sec      = new_water_schedule.duration_sec;
    schedule.of.water.interval_minutes  = new_water_schedule.interval_minutes;
    result = result_success;
  }

  return result;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_light_t_on_hour(uint8_t hour_turn_light_on)
{
  schedule.of.light.on_time.hour = hour_turn_light_on;
}


//--------------------------------------------------------------------------------------------------
uint8_t schedule_get_light_t_on_hour(void)
{
  return schedule.of.light.on_time.hour;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_light_t_on_min(uint8_t min_turn_light_on)
{
  schedule.of.light.on_time.min = min_turn_light_on;
}


//--------------------------------------------------------------------------------------------------
uint8_t schedule_get_light_t_on_min(void)
{
  return schedule.of.light.on_time.min;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_light_duration_hours(uint8_t light_on_duration)
{
  schedule.of.light.duration_sec = light_on_duration*3600;
}


//--------------------------------------------------------------------------------------------------
uint8_t schedule_get_light_duration_hours(void)
{
  uint8_t light_duration_hours = schedule.of.light.duration_sec/3600;
  return light_duration_hours;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_water_duration_sec(uint8_t water_duration_sec)
{
  schedule.of.water.duration_sec = water_duration_sec;
}


//--------------------------------------------------------------------------------------------------
uint32_t schedule_get_water_duration_sec(void)
{
  return schedule.of.water.duration_sec;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_water_interval_mins(uint8_t new_water_interval_mins)
{
  schedule.of.water.interval_minutes = new_water_interval_mins;
}


//--------------------------------------------------------------------------------------------------
uint32_t schedule_get_water_interval_mins(void)
{
  return schedule.of.water.interval_minutes;
}

