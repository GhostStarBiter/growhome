#include "main.h"



//--------------------------------------------------------------------------------------------------
void heartbeat_blink_led(void)
{
  static uint16_t led_timeout_counter = 0;
  static bool led_is_on = false;
  bool timeout_reached = false;

  led_timeout_counter++;

  timeout_reached = (led_timeout_counter % 200 == 0);

  if(timeout_reached)
  {
    if(led_is_on)
    {
      led_is_on = false;
      mcu_gpio_set_led_status(DISABLE);
      led_timeout_counter = 0;
    }
    else
    {
      led_is_on = true;
      mcu_gpio_set_led_status(ENABLE);
      led_timeout_counter = 0;
    }
  }
  else
  {
    // do nothing
  }
}


//--------------------------------------------------------------------------------------------------
void service_task(void *pvParameters)
{
  const TickType_t  x_service_task_timeout_ms = SERVICE_TASK_TIMEOUT;
  TickType_t        x_last_wake_time          = xTaskGetTickCount();

  for(;;)
  {
    // ***
    vTaskDelayUntil(&x_last_wake_time, x_service_task_timeout_ms);

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

//  // ****
  xTaskCreate(  system_user_interface_task,
                (const char *) "user_interface_task",
                configMINIMAL_STACK_SIZE,
                NULL,
                (UBaseType_t) USER_INTERFACE_TASK_PRIORITY,
                ( xTaskHandle * ) NULL);

  // ****
  vTaskStartScheduler();
}


