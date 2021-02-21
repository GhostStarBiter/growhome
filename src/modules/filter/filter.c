#include "filter_priv.h"


//--------------------------------------------------------------------------------------------------
void mean_filter_init(filter_object_t* p_filter_obj)
{
  p_filter_obj->filtered = 0;
  p_filter_obj->insert_index = 0;
  memset(p_filter_obj->measurement_buffer, 0, p_filter_obj->u8_window_size);
}


//--------------------------------------------------------------------------------------------------
void mean_filter_update(filter_object_t* p_filter, uint8_t u8_new_measurement)
{
  // @todo: check input parameters
  //  NULL pointer
  //  window size

  if(p_filter->insert_index < (p_filter->u8_window_size - 1))
  {
    p_filter->insert_index++;
  }
  else
  {
    p_filter->insert_index = 0;
  }

  uint8_t index = p_filter->insert_index;

  p_filter->measurement_buffer[index] = u8_new_measurement;

  mean_filter_calculate(p_filter);



}


//--------------------------------------------------------------------------------------------------
static void mean_filter_calculate(filter_object_t* p_filter)
{
  uint16_t u16_tmp = 0;
  for(uint8_t i = 0; i < p_filter->u8_window_size; i++)
  {
    u16_tmp += p_filter->measurement_buffer[i];
  }
  p_filter->filtered = u16_tmp / p_filter->u8_window_size;
}

