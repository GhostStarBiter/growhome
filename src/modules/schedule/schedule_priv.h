#ifndef SCHEDULE_PRIV_H_
#define SCHEDULE_PRIV_H_

#include "rtc/mcu_rtc.h"
#include "schedule.h"



#define WATER_CYCLES_PER_DAY                512

#define DAY_DEFAULT_LIGHT_DURATION_HOURS    12
#define DAY_DEFAULT_LIGHT_DURATION          DAY_DEFAULT_LIGHT_DURATION_HOURS*SECONDS_IN_HOUR

typedef struct{
    activity_schedule_t light;
    activity_schedule_t water;
} day_plan_t;

typedef struct{
  rtc_time_t  current_time;
  weekday_t   current_day;
  day_plan_t  day[DAYS_IN_WEEK];
} schedule_t;


//------------------------------------------------------------------------------
/// @brief  Init week schedule with default values
//------------------------------------------------------------------------------
static void schedule_init_week(void);


//------------------------------------------------------------------------------
/// @brief  Init week schedule with default values
//------------------------------------------------------------------------------
static void schedule_switch_date(void);


//------------------------------------------------------------------------------
/// @brief  Check schedule of light
//------------------------------------------------------------------------------
static void check_light_schedule(
    weekday_t       current_day,
    rtc_time_t      current_time,
    FunctionalState *greenhouse_light_state
);


//------------------------------------------------------------------------------
/// @brief  Check schedule of water
//------------------------------------------------------------------------------
static void check_water_schedule(
    weekday_t       current_day,
    rtc_time_t      current_time,
    FunctionalState *greenhouse_water_pump_state
);


#endif // SCHEDULE_PRIV_H_
