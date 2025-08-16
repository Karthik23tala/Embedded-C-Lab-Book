# FreeRTOS - Binary Semaphore

# Objective

To control an onboard LED using FreeRTOS and UART input on a PSoC 5LP board. The user needs to enter the required string “Toggle” in the Tera Term window and then the on board LED state is changed from LOW to HIGH or vice versa. 

# Semaphore - Definition

A semaphore in real time operating system like FreeRTOS is a signal or token which acts as a synchronization tool to co-ordinate between two or more tasks in the system or control the access to shared resources. Now, to access the resource or signal a particular task can take it, meaning waiting for it, or send it, meaning sending it, to other tasks using `xSemaphoretake()` and `xSemaphoreGive()` functions defined in FreeRTOS respectively. 

# Types of Semaphores - Binary and Counting Semaphore

Now, there are primarily two types of semaphores, namely binary and counting semaphore. The only difference lies in the number of resources available for sharing between the tasks. Binary semaphore has only two values either 0 or 1 whereas counting semaphore has values ranging from 0 to any positive number N depending on the number of resources available.

To relate with an analogy, a binary semaphore is like a doorbell where user presses it once to give a signal. On the other hand, counting semaphore is like a parking space in a basement. For instance, there are 5 parking spaces and initially the count will be 5. Once a car enters one of the spaces is occupied, the count is decremented. In this manner, when count equals 0 meaning the parking lot is full. If a car leaves the space, then the count is incremented to make space for another car that can park.

# Difference between Mutex and Binary Semaphore

In terms of the value range, both mutex and binary semaphore has only two values which is either 0 if the resource is occupied or 1 if the resource is available. The primary difference is that mutex supports priority inheritance (temporarily raising the priority of the lower priority which holds the resource) which helps to solve priority inversion and also mutex has recursive locking (one task can access the shared resource multiple times).hi

In simple words, a mutex can be compared to a key for a private room whereas binary semaphore can be considered as a doorbell on the main door. For mutex, only one person can take the key at a time and it protects the shared resource. For binary semaphore, no one owns the doorbell rather it is just like a signaling or notification for someone to know or alert.

# Working

In this lab exercise, we have defined two tasks namely `uart_input_task()` and `led_task()` using `xTaskCreate()` function. Among these two, UART task has higher priority of 3 as compared to the LED task of 2. When the user enters any string it is read using `UART_GetChar()` and echoed back to the Tera Term through `UART_PutChar()` function for user visibility. Once the user presses “Enter” key the program registers the string entered so far and case compares it with the “toggle” string using `strcasecmp()` function. If the result is TRUE, then the UART task prints “Toggle command received” and gives the semaphore to LED task which will toggle the state of LED post which both the tasks wait in the return state. If FALSE, then the UART prints “Unknown command”.

# PSoC-Schematic

<img width="865" height="516" alt="image" src="https://github.com/user-attachments/assets/8181415d-f67f-4c73-81c1-530cd6888d27" />

<img width="900" height="693" alt="image" src="https://github.com/user-attachments/assets/9638601e-a165-46e4-906e-f4062b4383b4" />

<img width="865" height="661" alt="image" src="https://github.com/user-attachments/assets/8245b947-de97-4823-843a-15b5c26276c6" />

# Pin Assignment

<img width="775" height="283" alt="image" src="https://github.com/user-attachments/assets/f4b7b96a-a7ed-4496-bdc0-d35731dffec5" />

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

SemaphoreHandle_t led_semaphore;

void uart_input_task(void *ptr) {
    char buffer[20];
    uint8_t index = 0;
    while(1) {
        char ch = UART_GetChar();
        if (ch != 0u && ch != 0xFF) {
            if (ch == '\r' || ch == '\n') {
                buffer[index] = '\0';
                index = 0;
                if (strcasecmp(buffer, "toggle") == 0) {
                    UART_PutString("\r\nToggle command received\r\n");
                    xSemaphoreGive(led_semaphore);
                } else {
                    UART_PutString("\r\nUnknown command\r\n");
                }
            } else if (index < sizeof(buffer) - 1) {
                buffer[index++] = ch;
                UART_PutChar(ch);
            }
        }
        vTaskDelay(10);
    }
}

void led_task(void *ptr) {
    while(1) {
        if(xSemaphoreTake(led_semaphore, portMAX_DELAY) == pdTRUE) {
            led_Write(!led_Read());
            UART_PutString(led_Read() ? "LED ON\r\n" : "LED OFF\r\n");
        }
    }
}

int main(void) {
    CyGlobalIntEnable;
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();
    led_semaphore = xSemaphoreCreateBinary();
    UART_PutString("Enter Toggle in the window to switch the LED state\r\n");
    if(led_semaphore == NULL) {
        UART_PutString("Failed to create semaphore\r\n");
        while(1);
    }
    xTaskCreate(uart_input_task, "UART Task", 200, NULL, 3, NULL);
    xTaskCreate(led_task, "LED Task", 200, NULL, 2, NULL);
    vTaskStartScheduler();
    for(;;) {}
}
```

Note: All the header, C files as well as `FreeRTOS.c` and `FreeRTOSConfig.h` need to be included for all the projects implemented through the FreeRTOS.

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

<img width="530" height="555" alt="image" src="https://github.com/user-attachments/assets/e05c7e5d-ffbb-46cd-9e1f-03168fcf7350" />

<img width="569" height="508" alt="image" src="https://github.com/user-attachments/assets/92327a3d-abb0-47a6-b218-e99cbf56899c" />

**— END**
