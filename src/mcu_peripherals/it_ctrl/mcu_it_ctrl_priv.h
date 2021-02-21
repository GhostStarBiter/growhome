#ifndef MCU_IT_CTRL_PRIV_H
#define MCU_IT_CTRL_PRIV_H

#include "misc.h"

#include "application_map.h"

#include "mcu_it_ctrl.h"

#define IRQ_GPOUR_IMPORTANT                           0
#define IRQ_GROUP_USUAL                               1


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void config_irq
(
    IRQn_Type irq,
    uint8_t preempt_prio,
    uint8_t sub_prio
);


#endif // MCU_IT_CTRL_PRIV_H
