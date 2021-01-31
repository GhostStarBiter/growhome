#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "stm32f10x.h"
#include "main/types.h"
#include "rtc/mcu_rtc.h"


typedef enum{
  SUNDAY,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY,
  DAYS_IN_WEEK
} weekday_t;

typedef struct{
    mcu_time_t  on_time;            // used for light/water schedule
    mcu_time_t  expected_endtime;   // used for light/water schedule control
    uint32_t    duration_sec;       // used for light/water schedule
    uint32_t    interval_minutes;   // used for water schedule
} activity_schedule_t;


//--------------------------------------------------------------------------------------------------
/// @brief  Init schedule module
//--------------------------------------------------------------------------------------------------
void schedule_init(void);


//--------------------------------------------------------------------------------------------------
/// @brief  Periodic check of schedule
/// @param
/// @param
//--------------------------------------------------------------------------------------------------
void schedule_update(
    FunctionalState *greenhouse_light_state,
    FunctionalState *greenhouse_water_pump_state
);


//--------------------------------------------------------------------------------------------------
/// @brief  API to set light day schedule from outside (e.g. ESP WI-FI)
/// @param
/// @param
//--------------------------------------------------------------------------------------------------
result_t schedule_set_light_schedule(
    weekday_t           of_week,
    activity_schedule_t new_light_schedule
);


//--------------------------------------------------------------------------------------------------
/// @brief  API to set water day schedule from outside (e.g. ESP WI-FI)
//--------------------------------------------------------------------------------------------------
result_t schedule_set_water_schedule(
    weekday_t           of_week,
    activity_schedule_t new_water_schedule
);



#endif // SCHEDULE_H_
