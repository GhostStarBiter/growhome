#include "mcu_rtc_priv.h"


//--------------------------------------------------------------------------------------------------
void mcu_rtc_init(void)
{
  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    RTC_WaitForSynchro();
    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Clear reset flags */
    RCC_ClearFlag();
  }
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_set_time(rtc_time_t set_time)
{
  if(set_time.hour > 23)
  {
    set_time.hour = 0;
  }
  if(set_time.min > 59)
  {
    set_time.min = 0;
  }

  uint32_t converted_time = set_time.hour*3600 + set_time.min*60;
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(converted_time);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}


//--------------------------------------------------------------------------------------------------
rtc_time_t mcu_rtc_get_time(void)
{
  rtc_time_t current_time;
  uint32_t rtc_counter;

  rtc_counter = RTC_GetCounter();

  current_time.hour = rtc_counter / 3600;
  current_time.min = (rtc_counter % 3600) / 60;
  current_time.sec = (rtc_counter % 3600) % 60;

  return current_time;
}


//--------------------------------------------------------------------------------------------------
void mcu_rtc_reset_counter(void)
{
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(0x00);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}
