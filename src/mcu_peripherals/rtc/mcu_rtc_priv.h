#ifndef MCU_RTC_PRIV_H
#define MCU_RTC_PRIV_H

#include "stm32f10x.h"
#include "system_config/stm32f10x_conf.h"

#include "mcu_rtc.h"

#define HOURS_PER_DAY                     24

#define SECONDS_PER_HOUR                  3600


typedef struct {
  uint32_t    counter;
  mcu_time_t  current;
  int         day;
} mcu_rtc_t;


#endif // MCU_RTC_PRIV_H
