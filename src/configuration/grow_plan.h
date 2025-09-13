#ifndef GROW_PLAN_H
#define GROW_PLAN_H

#include "schedule/schedule.h"
#include "green_house_system/green_house.h"

typedef enum {
  SEED_PERIOD = 7,    // [days]
  SPROUT_PERIOD = 14, // [days]
  PLANT_PERIOD = 28   // [days]
} grow_plan_periods;

void grow_plan_update_settings( grow_mode_t grow_mode,
                                double *temperature,
                                activity_schedule_t *light,
                                activity_schedule_t *water)
{
  switch(grow_mode)
  {
    case SEED:
      *temperature = 23;

      // no light for seeds
      light->duration_sec = 0;

      water->on_time.hour = 0;
      water->on_time.min = 0;
      water->on_time.sec = 0;
      water->interval_minutes = 6*60;  // water the seeds one time for 6 hours
      water->duration_sec = 10;
      break;

    case SPROUT:
      *temperature = 26;

      light->on_time.hour = 6;
      light->on_time.min = 0;
      light->on_time.sec = 0;
      light->duration_sec = 14*60*60;

      water->on_time.hour = 6;
      water->on_time.min = 0;
      water->on_time.sec = 0;
      water->interval_minutes = 3*60;  // water the sprouts one time for 3 hours
      water->duration_sec = 15;
      break;

    case PLANT:
      *temperature = 28;

      light->on_time.hour = 6;
      light->on_time.min = 0;
      light->on_time.sec = 0;
      light->duration_sec = GROWBOX_MAX_LIGHT_ON_HOURS*60*60;

      water->on_time.hour = 6;
      water->on_time.min = 0;
      water->on_time.sec = 0;
      water->interval_minutes = 3*60;  // water the plant one time for 3 hours
      water->duration_sec = 15;
      break;
  }
}



#endif // GROW_PLAN_H
