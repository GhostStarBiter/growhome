#ifndef FILTER_H
#define FILTER_H

typedef struct {
    uint8_t*    measurement_buffer;
    uint8_t     insert_index;
    uint8_t     u8_window_size;
    uint16_t    filtered;
} filter_object_t;


//--------------------------------------------------------------------------------------------------
void mean_filter_init
(
    filter_object_t* p_filter_obj
);

//--------------------------------------------------------------------------------------------------
void mean_filter_update
(
    filter_object_t* p_filter,
    uint8_t u8_new_measurement
);




#endif // FILTER_H
