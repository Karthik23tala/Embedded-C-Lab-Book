# FreeRTOS - Mutex

# Objective

To define two tasks in FreeRTOS which toggles the on board LED and prints the appropriate message on Tera Term through UART. Define a mutex to ensure proper mutual exclusion between both the tasks as UART is the shared peripheral on the board.

# Mutex - Definition

Mutex itself stands as an acronym of mutual exclusion meaning it is used for shared resource between two or more tasks. Basically it ensures a proper synchronization in a multi tasking system. In other words, mutex can be treated as a special type of binary semaphore which also has priority inheritance. In RTOS, priority inheritance means it prevents priority inversion (low-priority task holding a mutex boosts to higher priority). 

To understand the working of a mutex, consider a real life scenario wherein two people share the room with only single bathroom. The lock for the bathroom can be considered as a mutex. If you use the bathroom then you will lock it and your roommate will wait until it is finished and you unlock the door. 

Similarly, in the below code example, lock is the mutex, bathroom is the shared resource i.e., UART, two people in the room are task 1 and task 2. Now, in coding, the locking and unlocking of the mutex is done using the `xSemaphoreTake()` and `xSemaphoreGive()` functions respectively.

# Working

In this lab exercise, two tasks named `task1()` and `task2()` are created using `xTaskCreate()` function in which the LED pin is toggled infinitely under the `for(;;)` super loop. As a simple definition, both the tasks are toggling the same on board LED and printing the appropriate LED ON or LED OFF message on the Tera Term app through the UART. But since both the tasks cannot write onto UART on the same time, otherwise the message will be corrupted, a mutex is defined in the code to handle it. Mutex ensures only one task writes to UART at a time.

# PSoC-Schematic

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%201.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%202.png)

# Pin Assignment

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%203.png)

Here Pin P2[1] is fixed on the PSoC board itself for LED. In case you want to use any external LED or display unit, assign the appropriate GPIO for the same.

# PSoC-Code

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern void FreeRTOS_Start();

SemaphoreHandle_t uart_mutex;

void task1(void *ptr) {
    for(;;) {
        if (xSemaphoreTake(uart_mutex, portMAX_DELAY) == pdTRUE) {
            Pin_LED_Write(!Pin_LED_Read());
            UART_PutString("Task 1: LED ");
            UART_PutString(Pin_LED_Read() ? "ON\r\n" : "OFF\r\n");
            xSemaphoreGive(uart_mutex);
        }
        vTaskDelay(500);
    }
}

void task2(void *ptr) {
    for(;;) {
        if (xSemaphoreTake(uart_mutex, portMAX_DELAY) == pdTRUE) {
            Pin_LED_Write(!Pin_LED_Read());
            UART_PutString("Task 2: LED ");
            UART_PutString(Pin_LED_Read() ? "ON\r\n" : "OFF\r\n");
            xSemaphoreGive(uart_mutex);
        }
        vTaskDelay(750);
    }
}

int main(void) {
    CyGlobalIntEnable;
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();
    uart_mutex = xSemaphoreCreateMutex();
    if(uart_mutex == NULL) {
        UART_PutString("Failed to create mutex\r\n");
        while(1);
    }
    xTaskCreate(task1, "Task 1", 200, NULL, 2, NULL);
    xTaskCreate(task2, "Task 2", 200, NULL, 2, NULL);
    vTaskStartScheduler();
    for(;;) {}
}
```

Note: All the header, C files as well as `FreeRTOS.c` and `FreeRTOSConfig.h` need to be included for all the projects implemented through the FreeRTOS.

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

![image.png](Addition%20without%20Pointers%20206d051c359e80278902dfcca222e0eb/image%203.png)

![image.png](FreeRTOS%20-%20Mutex%20231d051c359e8025b8c6e84460660c48/image.png)

**— END**