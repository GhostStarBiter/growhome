#include "schedule_priv.h"

schedule_t schedule;

//--------------------------------------------------------------------------------------------------
void schedule_init(void)
{
  schedule.current_day        = SUNDAY;

  schedule.current_time.hour  = 6;
  schedule.current_time.min   = 55;

  //mcu_rtc_set_time(schedule.current_time);

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

    schedule.day[weekday].water.duration_sec     = 30;
    schedule.day[weekday].water.interval_minutes = 4;
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
  // check day's 24 hours elapsed
  if(current_time.hour > 23)
  {
    mcu_rtc_reset_counter();
    schedule_switch_date();
    current_time = mcu_rtc_get_time();
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
  // Check LIGHT schedule for today
  if( *greenhouse_light_state != ENABLE &&
      schedule.day[current_day].light.on_time.hour <= current_time.hour &&
      schedule.day[current_day].light.on_time.min <= current_time.min)
  {
    *greenhouse_light_state = ENABLE;

    // convert duration to minutes
    schedule.day[current_day].light.expected_endtime.min =
        schedule.day[current_day].light.duration_sec / 60;
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

  if(*greenhouse_light_state == ENABLE &&
      schedule.day[current_day].light.expected_endtime.hour <= current_time.hour &&
      schedule.day[current_day].light.expected_endtime.min <= current_time.min)
  {
    *greenhouse_light_state = DISABLE;
  }
}


//--------------------------------------------------------------------------------------------------
static void check_water_schedule(
    weekday_t       current_day,
    mcu_time_t      current_time,
    FunctionalState *greenhouse_water_pump_state)
{

  // Check WATER schedule for today
  if( *greenhouse_water_pump_state != ENABLE &&
      schedule.day[current_day].water.on_time.hour <= current_time.hour &&
      schedule.day[current_day].water.on_time.min <= current_time.min &&
      schedule.day[current_day].water.on_time.sec <= current_time.sec)
  {
    *greenhouse_water_pump_state = ENABLE;

    // extract duration hours
    schedule.day[current_day].water.expected_endtime.hour =
        schedule.day[current_day].water.duration_sec / 3600;
    // convert duration to minutes
    schedule.day[current_day].water.expected_endtime.min  =
        schedule.day[current_day].water.duration_sec / 60;
    // define duration seconds
    schedule.day[current_day].water.expected_endtime.sec  =
        schedule.day[current_day].water.duration_sec % 60;

    schedule.day[current_day].water.expected_endtime.hour +=
        schedule.day[current_day].water.on_time.hour;

    schedule.day[current_day].water.expected_endtime.min  +=
        schedule.day[current_day].water.on_time.min;

    schedule.day[current_day].water.expected_endtime.sec  +=
        schedule.day[current_day].water.on_time.sec;

    if(schedule.day[current_day].water.expected_endtime.sec > 59)
    {
      schedule.day[current_day].water.expected_endtime.sec %= 60;
      schedule.day[current_day].water.expected_endtime.min++;
    }
    if(schedule.day[current_day].water.expected_endtime.min > 59)
    {
      schedule.day[current_day].water.expected_endtime.min %= 60;
      schedule.day[current_day].water.expected_endtime.hour++;
    }

    if(schedule.day[current_day].water.expected_endtime.hour > 23)
    {
      schedule.day[current_day].water.expected_endtime.hour = 23;
      schedule.day[current_day].water.expected_endtime.min = 59;
      schedule.day[current_day].water.expected_endtime.sec = 59;
    }
  }

  if(*greenhouse_water_pump_state == ENABLE &&
      schedule.day[current_day].water.expected_endtime.hour <= current_time.hour &&
      schedule.day[current_day].water.expected_endtime.min <= current_time.min &&
      schedule.day[current_day].water.expected_endtime.sec <= current_time.sec)
  {
    *greenhouse_water_pump_state = DISABLE;

    // update next ON time according to interval
    schedule.day[current_day].water.on_time.min =
        schedule.day[current_day].water.expected_endtime.min +
        schedule.day[current_day].water.interval_minutes;

    schedule.day[current_day].water.on_time.hour =
        schedule.day[current_day].water.expected_endtime.hour;

    if(schedule.day[current_day].water.on_time.min > 59)
    {
      schedule.day[current_day].water.on_time.min %= 60;
      schedule.day[current_day].water.on_time.hour++;
    }
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
