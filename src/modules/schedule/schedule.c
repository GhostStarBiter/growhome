#include "schedule_priv.h"

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

  schedule.current_day        = SUNDAY;

  schedule.current_time.hour  = init_time.hour;
  schedule.current_time.min   = init_time.min;

  schedule_init_week();
}


//--------------------------------------------------------------------------------------------------
static void schedule_init_week(void)
{
  for(weekday_t weekday = SUNDAY; weekday < DAYS_IN_WEEK; weekday++)
  {
    schedule.day[weekday].light.on_time.hour     = 7;
    schedule.day[weekday].light.on_time.min      = 0;
    schedule.day[weekday].light.duration_sec     = DAY_DEFAULT_LIGHT_DURATION;

    schedule.day[weekday].water.on_time.hour     = 7;
    schedule.day[weekday].water.on_time.min      = 0;

    schedule.day[weekday].water.duration_sec     = WATER_T_ON_SEC_DEFAULT;
    schedule.day[weekday].water.interval_minutes = WATER_INTERVAL_MINS_DEFAULT;
  }
}


//--------------------------------------------------------------------------------------------------
void schedule_update
(
    FunctionalState *greenhouse_light_state,
    FunctionalState *greenhouse_water_pump_state
)
{
  mcu_time_t current_time = mcu_rtc_get_time();

  if(current_time.hour == 00
      && current_time.min == 00
      && current_time.sec == 00)
  {
    schedule_switch_date();
  }


  check_light_schedule(
      schedule.current_day,
      current_time,
      greenhouse_light_state
  );


  check_water_schedule(
      schedule.current_day,
      current_time,
      greenhouse_water_pump_state
  );

}


//--------------------------------------------------------------------------------------------------
static void schedule_switch_date(void)
{
  schedule.current_day++;

  if(schedule.current_day > SATURDAY)
  {
    schedule.current_day = SUNDAY;
  }
}


//--------------------------------------------------------------------------------------------------
static void check_light_schedule(
    weekday_t       current_day,
    mcu_time_t      current_time,
    FunctionalState *greenhouse_light_state)
{
  uint32_t minutes_schedule, minutes_current_time;

  minutes_schedule = schedule.day[current_day].light.on_time.hour*60 + schedule.day[current_day].light.on_time.min;
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

  minutes_schedule = schedule.day[current_day].light.expected_endtime.hour*60 + schedule.day[current_day].light.expected_endtime.min;

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
  weekday_t current_day = schedule.current_day;
  // convert duration to minutes
  schedule.day[current_day].light.expected_endtime.min =
      schedule.day[current_day].light.duration_sec % 60;
  // extract duration hours
  schedule.day[current_day].light.expected_endtime.hour  =
      schedule.day[current_day].light.duration_sec / 3600;

  schedule.day[current_day].light.expected_endtime.min   +=
      schedule.day[current_day].light.on_time.min;

  schedule.day[current_day].light.expected_endtime.hour  +=
      schedule.day[current_day].light.on_time.hour;

  if(schedule.day[current_day].light.expected_endtime.min > 59)
  {
    schedule.day[current_day].light.expected_endtime.min %= 60;
    schedule.day[current_day].light.expected_endtime.hour++;
  }

  if(schedule.day[current_day].light.expected_endtime.hour > 23)
  {
    schedule.day[current_day].light.expected_endtime.hour = 23;
    schedule.day[current_day].light.expected_endtime.min = 59;
  }
}


//--------------------------------------------------------------------------------------------------
static void check_water_schedule(
    weekday_t       current_day,
    mcu_time_t      current_time,
    FunctionalState *greenhouse_water_pump_state)
{
  bool new_interval_started;

  uint32_t minutes_current_time, seconds_current_time;

  minutes_current_time = current_time.hour*60 + current_time.min;

  seconds_current_time = minutes_current_time*60 + current_time.sec;

  new_interval_started = seconds_current_time % (schedule.day[current_day].water.interval_minutes*60);

  if(!new_interval_started)
  {
    schedule.day[current_day].water.expected_endtime.sec = seconds_current_time + schedule.day[current_day].water.duration_sec;
    *greenhouse_water_pump_state = ENABLE;
  }

  if(seconds_current_time >= schedule.day[current_day].water.expected_endtime.sec)
  {
    *greenhouse_water_pump_state = DISABLE;

  }

}


//--------------------------------------------------------------------------------------------------
result_t schedule_set_light_schedule(
    weekday_t           of_week,
    activity_schedule_t new_light_schedule
)
{
  result_t result = result_fail;

  if( of_week < DAYS_IN_WEEK &&
      new_light_schedule.on_time.hour < 23 &&
      new_light_schedule.on_time.min < 59 &&
      new_light_schedule.on_time.sec < 59 &&
      new_light_schedule.duration_sec < 24 * SECONDS_IN_HOUR)
  {
    schedule.day[of_week].light.on_time       = new_light_schedule.on_time;
    schedule.day[of_week].light.duration_sec  = new_light_schedule.duration_sec;
    result = result_success;
  }

  return result;
}


//--------------------------------------------------------------------------------------------------
result_t schedule_set_water_schedule(
    weekday_t           of_week,
    activity_schedule_t new_water_schedule
)
{
  result_t result = result_fail;

  if( of_week < DAYS_IN_WEEK &&
      new_water_schedule.on_time.hour < 23 &&
      new_water_schedule.on_time.min < 59 &&
      new_water_schedule.on_time.sec < 59 &&
      new_water_schedule.duration_sec < 24 * SECONDS_IN_HOUR &&
      new_water_schedule.interval_minutes != 0)
  {
    schedule.day[of_week].water.on_time           = new_water_schedule.on_time;
    schedule.day[of_week].water.duration_sec      = new_water_schedule.duration_sec;
    schedule.day[of_week].water.interval_minutes  = new_water_schedule.interval_minutes;
    result = result_success;
  }

  return result;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_light_t_on_hour(uint8_t hour_turn_light_on)
{
  weekday_t current_day = schedule.current_day;
  schedule.day[current_day].light.on_time.hour = hour_turn_light_on;
}


//--------------------------------------------------------------------------------------------------
uint8_t schedule_get_light_t_on_hour(void)
{
  weekday_t current_day = schedule.current_day;
  return schedule.day[current_day].light.on_time.hour;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_light_t_on_min(uint8_t min_turn_light_on)
{
  weekday_t current_day = schedule.current_day;
  schedule.day[current_day].light.on_time.min = min_turn_light_on;
}


//--------------------------------------------------------------------------------------------------
uint8_t schedule_get_light_t_on_min(void)
{
  weekday_t current_day = schedule.current_day;
  return schedule.day[current_day].light.on_time.min;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_light_duration_hours(uint8_t light_on_duration)
{
  weekday_t current_day = schedule.current_day;
  schedule.day[current_day].light.duration_sec = light_on_duration*3600;
}


//--------------------------------------------------------------------------------------------------
uint8_t schedule_get_light_duration_hours(void)
{
  weekday_t current_day = schedule.current_day;
  uint8_t light_duration_hours = schedule.day[current_day].light.duration_sec/3600;
  return light_duration_hours;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_water_duration_sec(uint8_t water_duration_sec)
{
  weekday_t current_day = schedule.current_day;
  schedule.day[current_day].water.duration_sec = water_duration_sec;
}


//--------------------------------------------------------------------------------------------------
uint32_t schedule_get_water_duration_sec(void)
{
  weekday_t current_day = schedule.current_day;
  return schedule.day[current_day].water.duration_sec;
}


//--------------------------------------------------------------------------------------------------
void schedule_set_water_interval_mins(uint8_t new_water_interval_mins)
{
  weekday_t current_day = schedule.current_day;
  schedule.day[current_day].water.interval_minutes = new_water_interval_mins;
}


//--------------------------------------------------------------------------------------------------
uint32_t schedule_get_water_interval_mins(void)
{
  weekday_t current_day = schedule.current_day;
  return schedule.day[current_day].water.interval_minutes;
}

