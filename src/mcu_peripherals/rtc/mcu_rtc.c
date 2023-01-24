#include "stm32f10x.h"
#include "system_config/stm32f10x_conf.h"
#include "mcu_rtc.h"

#define HOURS_PER_DAY                     24

#define SECONDS_PER_HOUR                  3600

typedef struct {
  void (*day_change_callback)(int);
  uint32_t    counter;
  mcu_time_t  current;
  uint16_t    day;
} mcu_rtc_t;

volatile mcu_rtc_t mcu_rtc;

//--------------------------------------------------------------------------------------------------
void mcu_rtc_init(void(*new_day_callback)(int))
{
  mcu_rtc.day = 1;
  mcu_rtc_reset_counter();
  mcu_rtc.day_change_callback = new_day_callback;
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_update(void)
{
  mcu_rtc.counter++;

  if(mcu_rtc.counter >= (HOURS_PER_DAY*SECONDS_PER_HOUR))
  {
    mcu_rtc.day++;
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
uint16_t mcu_rtc_get_day(void)
{
  return mcu_rtc.day;
}

void mcu_rtc_set_day(uint16_t day_cnt)
{
  mcu_rtc.day = day_cnt;
  mcu_rtc.day_change_callback(day_cnt);
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
