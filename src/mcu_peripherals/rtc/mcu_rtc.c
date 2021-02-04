#include "mcu_rtc_priv.h"


volatile mcu_rtc_t mcu_time;

//--------------------------------------------------------------------------------------------------
void mcu_rtc_init(void)
{
  mcu_rtc_reset_counter();
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_update(void)
{
  mcu_time.counter++;

  if(mcu_time.counter >= (24*3600 + 60*60 + 60*60))
  {
    mcu_rtc_reset_counter();
  }

  mcu_time.current.hour = mcu_time.counter / 3600;
  mcu_time.current.min = (mcu_time.counter % 3600) / 60;
  mcu_time.current.sec = (mcu_time.counter % 3600) % 60;
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
  if(set_time.sec > 60)
  {
    set_time.sec = 0;
  }

  mcu_time.counter = set_time.hour*3600 + set_time.min*60 + set_time.sec;
}


//--------------------------------------------------------------------------------------------------
mcu_time_t mcu_rtc_get_time(void)
{
  mcu_time_t current_time;

  current_time.hour = mcu_time.counter / 3600;
  current_time.min = (mcu_time.counter % 3600) / 60;
  current_time.sec = (mcu_time.counter % 3600) % 60;

  return current_time;
}


//--------------------------------------------------------------------------------------------------
uint8_t mcu_rtc_get_hour(void)
{
  return mcu_time.current.hour;
}


//--------------------------------------------------------------------------------------------------
uint8_t mcu_rtc_get_minutes(void)
{
  return mcu_time.current.min;
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_reset_counter(void)
{
  mcu_time.counter = 0;
}
