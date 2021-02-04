#ifndef MCU_RTC_H_
#define MCU_RTC_H_

#define MINUTES_IN_HOUR                60
#define SECONDS_IN_HOUR                (MINUTES_IN_HOUR*60)

typedef struct{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} mcu_time_t;


//------------------------------------------------------------------------------
/// @brief
//------------------------------------------------------------------------------
void mcu_rtc_init(void);


//------------------------------------------------------------------------------
/// @brief  Update function to be called every 1 sec
//------------------------------------------------------------------------------
void mcu_rtc_update(void);


//------------------------------------------------------------------------------
/// @brief
/// @param
//------------------------------------------------------------------------------
void mcu_rtc_set_time
(
  mcu_time_t set_time
);


//------------------------------------------------------------------------------
/// @brief
/// @return
//------------------------------------------------------------------------------
mcu_time_t mcu_rtc_get_time(void);


//------------------------------------------------------------------------------
/// @brief
/// @return
//------------------------------------------------------------------------------
uint8_t mcu_rtc_get_hour(void);


//------------------------------------------------------------------------------
/// @brief
/// @return
//------------------------------------------------------------------------------
uint8_t mcu_rtc_get_minutes(void);


//------------------------------------------------------------------------------
/// @brief  Reset the RTC counter value when 24 hours elapsed
//------------------------------------------------------------------------------
void mcu_rtc_reset_counter(void);


#endif // MCU_RTC_H_
