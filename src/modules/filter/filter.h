#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    double*     measurement_buffer;
    uint8_t     insert_index;
    uint8_t     window_size;
    double      filtered;
} filter_object_t;


//--------------------------------------------------------------------------------------------------
void mean_filter_init
(
    filter_object_t* p_filter_obj
);

//--------------------------------------------------------------------------------------------------
void mean_filter_update
(
    filter_object_t*  p_filter,
    double            new_measurement
);




#endif // FILTER_H
