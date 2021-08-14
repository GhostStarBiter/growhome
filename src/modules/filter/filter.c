#include "filter_priv.h"


//--------------------------------------------------------------------------------------------------
void mean_filter_init(filter_object_t* p_filter_obj)
{
  // assert for null pointer
  // measurement_buffer pointer for filter object
  // must be set before call to filter init function
  while(!p_filter_obj->measurement_buffer);

  p_filter_obj->filtered = 0;
  p_filter_obj->insert_index = 0;
  memset(p_filter_obj->measurement_buffer, 0, p_filter_obj->window_size);
}


//--------------------------------------------------------------------------------------------------
void mean_filter_update(filter_object_t* p_filter, double new_measurement)
{
  if(p_filter->insert_index < (p_filter->window_size - 1))
  {
    p_filter->insert_index++;
  }
  else
  {
    p_filter->insert_index = 0;
  }

  uint8_t index = p_filter->insert_index;

  p_filter->measurement_buffer[index] = new_measurement;

  mean_filter_calculate(p_filter);

}


//--------------------------------------------------------------------------------------------------
static void mean_filter_calculate(filter_object_t* p_filter)
{
  uint32_t tmp = 0;

  for(uint8_t i = 0; i < p_filter->window_size; i++)
  {
    tmp += p_filter->measurement_buffer[i];
  }
  p_filter->filtered = tmp / p_filter->window_size;
}

