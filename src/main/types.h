#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define ZERO                                0

typedef enum{
  result_fail = 0,
  result_success
} result_t;


// Double linked list
typedef struct linked_list{
    struct linked_list* prev;
    struct linked_list* next;
} llist_t;


typedef enum {
  WATER_PUMP  = 1,
  AIR_INPUT   = 2
} PWM_Channel_t;

#endif  // TYPES_H
