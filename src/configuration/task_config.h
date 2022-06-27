#ifndef TASK_CONFIG_H
#define TASK_CONFIG_H

// ***
// Task priorities
//  the higher important
#define SERVICE_TASK_PRIORITY               1
#define NETWORK_COMM_TASK_PRIORITY          2
#define USER_INTERFACE_TASK_PRIORITY        3
#define GROWBOX_TASK_PRIORITY               4

// ***
// Task call intervals
#define INTERFACE_TASK_CYCLE                10      // [ms]
#define SERVICE_TASK_CYCLE                  1000    // [ms]
#define GROWBOX_TASK_CYCLE                  1       // [ms]

#endif // TASK_CONFIG_H
