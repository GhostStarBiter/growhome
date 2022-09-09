#include <string.h>

// ***
#include "stm32f10x.h"
#include <system_config/FreeRTOSConfig.h>
#include "FreeRTOS.h"
#include "task.h"

// ***
#include "types.h"
#include "configuration/app_config.h"
#include "configuration/task_config.h"

// ***
#include "mcu_peripherals.h"

// ***
#include "system_user_interface/sui.h"

// ***
#include "green_house_system/green_house.h"

// ***
#include "network/network.h"

//--------------------------------------------------------------------------------------------------
static void heartbeat_blink_led(void)
{
  static bool led_is_on = false;

  if(led_is_on)
  {
    led_is_on = false;
    mcu_gpio_set_led_status(DISABLE);
  }
  else
  {
    led_is_on = true;
    mcu_gpio_set_led_status(ENABLE);
  }

}


//--------------------------------------------------------------------------------------------------
static void service_task(void *pvParameters)
{
  const TickType_t  x_service_task_cycle_ms = SERVICE_TASK_CYCLE;
  TickType_t        x_last_wake_time          = xTaskGetTickCount();

  for(;;)
  {
    // ***
    vTaskDelayUntil(&x_last_wake_time, x_service_task_cycle_ms);

    // ***
    mcu_rtc_update();

    // ***
    heartbeat_blink_led();
  };
}


//_________________________________________________________________________________________________
int main(void)
{
  // ****
  mcu_init_peripherals();

  // ****
  xTaskCreate(  service_task,
                (const char *) "service_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                (UBaseType_t) SERVICE_TASK_PRIORITY,
                ( xTaskHandle * ) NULL);

  // ****
  xTaskCreate(  growbox_control_rtos_task,
                (const char *) "growbox_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                (UBaseType_t) GROWBOX_TASK_PRIORITY,
                ( xTaskHandle * ) NULL);

  // ****
  xTaskCreate(  system_user_interface_task,
                (const char *) "user_interface_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                (UBaseType_t) USER_INTERFACE_TASK_PRIORITY,
                ( xTaskHandle * ) NULL);

#if GRWHS_USE_NETWORK
  // ****
  xTaskCreate(  network_communication_task,
                (const char *) "network_task",
                1024,
                NULL,
                (UBaseType_t) NETWORK_COMM_TASK_PRIORITY,
                ( xTaskHandle * ) NULL);
#endif

  // ****
  vTaskStartScheduler();
}


