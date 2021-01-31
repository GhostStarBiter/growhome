#ifndef MCU_RTC_PRIV_H
#define MCU_RTC_PRIV_H

#include "stm32f10x.h"
#include "system_config/stm32f10x_conf.h"

#include "mcu_rtc.h"

typedef struct {
  uint32_t    counter;
  mcu_time_t  current;
} mcu_rtc_t;


#endif // MCU_RTC_PRIV_H
