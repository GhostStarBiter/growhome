#include "mcu_rtc_priv.h"


volatile mcu_rtc_t mcu_rtc;

//--------------------------------------------------------------------------------------------------
void mcu_rtc_init(void)
{
  mcu_rtc.day = 1;
  mcu_rtc_reset_counter();
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_update(void)
{
  mcu_rtc.counter++;

  if(mcu_rtc.counter >= (HOURS_PER_DAY*SECONDS_PER_HOUR))
  {
    mcu_rtc_reset_counter();
  }

  mcu_rtc.current.hour = mcu_rtc.counter / SECONDS_PER_HOUR;
  mcu_rtc.current.min = (mcu_rtc.counter % SECONDS_PER_HOUR) / 60;
  mcu_rtc.current.sec = (mcu_rtc.counter % SECONDS_PER_HOUR) % 60;
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_set_time(mcu_time_t set_time)
{
  if(set_time.hour > 23)
  {
    set_time.hour = 0;
  }
  if(set_time.min > 59)
  {
    set_time.min = 0;
  }
  if(set_time.sec > 59)
  {
    set_time.sec = 0;
  }

  mcu_rtc.counter = set_time.hour*3600 + set_time.min*60 + set_time.sec;
}


//--------------------------------------------------------------------------------------------------
mcu_time_t mcu_rtc_get_time(void)
{
  return mcu_rtc.current;
}


//--------------------------------------------------------------------------------------------------
uint8_t mcu_rtc_get_hour(void)
{
  return mcu_rtc.current.hour;
}


//--------------------------------------------------------------------------------------------------
uint8_t mcu_rtc_get_minutes(void)
{
  return mcu_rtc.current.min;
}

//--------------------------------------------------------------------------------------------------
uint8_t mcu_rtc_get_seconds(void)
{
  return mcu_rtc.current.sec;
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_reset_counter(void)
{
  mcu_rtc.counter = 0;
}
