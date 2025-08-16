# FreeRTOS - Counting Semaphore

# Objective

To control an onboard LED using FreeRTOS and UART input on a PSoC 5LP board. Apart from the single TOGGLE command in binary semaphore, over here user can choose from the multiple options displayed and perform the respective action on the board.

# Semaphore - Definition

A semaphore in real time operating system like FreeRTOS is a signal or token which acts as a synchronization tool to co-ordinate between two or more tasks in the system or control the access to shared resources. Now, to access the resource or signal a particular task can take it, meaning waiting for it, or send it, meaning sending it, to other tasks using `xSemaphoretake()` and `xSemaphoreGive()` functions defined in FreeRTOS respectively. 

# Types of Semaphores - Binary and Counting Semaphore

Now, there are primarily two types of semaphores, namely binary and counting semaphore. The only difference lies in the number of resources available for sharing between the tasks. Binary semaphore has only two values either 0 or 1 whereas counting semaphore has values ranging from 0 to any positive number N depending on the number of resources available.

To relate with an analogy, a binary semaphore is like a doorbell where user presses it once to give a signal. On the other hand, counting semaphore is like a parking space in a basement. For instance, there are 5 parking spaces and initially the count will be 5. Once a car enters one of the spaces is occupied, the count is decremented. In this manner, when count equals 0 meaning the parking lot is full. If a car leaves the space, then the count is incremented to make space for another car that can park.

# Working

In the given lab exercise, there are three tasks created to start with, namely UART input task, LED task and auto task using `xTaskCreate()` function. Now, the UART input task is nothing but defined to take the input from the user in tera term and echo back the characters for user visibility. Next up, auto task is to give a semaphore for every one second. At the end, led task is where all the commands apart from AUTO ON and AUTO OFF are taken up. To discuss all the commands one by one, TOGGLE command gives the semaphore 3 times which adds 3 toggle events, AUTO ON will make the `auto_mode = 1` meaning the background task starts sending 1 event/sec via semaphore. On the other hand, AUTO OFF will make the `auto_mode = 0` which will stop the automatic events. Now, ON and OFF will simply turn the led on or off respectively. STATUS command shows the LED status and also the count of auto, manual and total events so far whereas RESET command will reset the counter to zero. Lastly, SLEEP command will put the LED to sleep and make `led_enabled = 0` which means no commands will be taken in unless the led is waken up using the NORMAL command that makes `led_enabled = 1`. 

# PSoC-Schematic

<img width="865" height="516" alt="image" src="https://github.com/user-attachments/assets/330cb9f5-27ec-4533-99ab-6b3f368e4f56" />

<img width="900" height="693" alt="image" src="https://github.com/user-attachments/assets/80949f1d-fbbf-4b42-819d-bb2026a041f6" />

<img width="865" height="661" alt="image" src="https://github.com/user-attachments/assets/4b1fe07e-33f3-4718-a3ec-7da45b84c591" />

# Pin Assignment

<img width="775" height="283" alt="image" src="https://github.com/user-attachments/assets/a6a5710a-4d07-4a08-aa38-957ae392a8f5" />

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

SemaphoreHandle_t toggle_semaphore;

volatile uint8_t led_enabled = 1;
volatile uint8_t auto_mode = 0;
volatile uint32_t manual_event_count = 0;
volatile uint32_t auto_event_count = 0;
volatile uint32_t total_events_handled = 0;

void uart_input_task(void *ptr) {
    char buffer[32];
    uint8_t index = 0;
    while (1) {
        char ch = UART_GetChar();
        if (ch != 0u && ch != 0xFF) {
            if (ch == '\r' || ch == '\n') {
                buffer[index] = '\0';
                index = 0;

                if (strcasecmp(buffer, "toggle") == 0) {
                    if (led_enabled) {
                        for (int i = 0; i < 3; i++) {
                            xSemaphoreGive(toggle_semaphore);
                            manual_event_count++;
                        }
                        UART_PutString("\r\n[Manual] 3 toggle events queued\r\n");
                    } else {
                        UART_PutString("\r\nLED disabled (sleep mode)\r\n");
                    }
                } else if (strcasecmp(buffer, "auto on") == 0) {
                    auto_mode = 1;
                    UART_PutString("\r\nAuto events enabled\r\n");
                } else if (strcasecmp(buffer, "auto off") == 0) {
                    auto_mode = 0;
                    UART_PutString("\r\nAuto events disabled\r\n");
                } else if (strcasecmp(buffer, "status") == 0) {
                    char status[128];
                    sprintf(status,
                        "\r\nLED is %s\r\nManual events: %lu\r\nAuto events: %lu\r\nTotal handled: %lu\r\n",
                        led_Read() ? "ON" : "OFF",
                        manual_event_count,
                        auto_event_count,
                        total_events_handled
                    );
                    UART_PutString(status);
                } else if (strcasecmp(buffer, "reset") == 0) {
                    manual_event_count = 0;
                    auto_event_count = 0;
                    total_events_handled = 0;
                    UART_PutString("\r\nCounters reset\r\n");
                } else if (strcasecmp(buffer, "on") == 0) {
                    led_Write(1);
                    UART_PutString("\r\nLED forced ON\r\n");
                } else if (strcasecmp(buffer, "off") == 0) {
                    led_Write(0);
                    UART_PutString("\r\nLED forced OFF\r\n");
                } else if (strcasecmp(buffer, "sleep") == 0) {
                    led_enabled = 0;
                    UART_PutString("\r\nLED sleep mode activated\r\n");
                } else if (strcasecmp(buffer, "normal") == 0) {
                    led_enabled = 1;
                    UART_PutString("\r\nLED toggle re-enabled\r\n");
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
    while (1) {
        if (xSemaphoreTake(toggle_semaphore, portMAX_DELAY) == pdTRUE) {
            if (led_enabled) {
                led_Write(!led_Read());
                total_events_handled++;
                char msg[64];
                sprintf(msg, "Event #%lu -> LED %s\r\n",
                        total_events_handled,
                        led_Read() ? "ON" : "OFF");
                UART_PutString(msg);
            } else {
                UART_PutString("Event ignored (LED in sleep mode)\r\n");
            }
            vTaskDelay(250);
        }
    }
}

void auto_event_task(void *ptr) {
    while (1) {
        if (auto_mode) {
            xSemaphoreGive(toggle_semaphore);
            auto_event_count++;
        }
        vTaskDelay(1000); // One auto event every second
    }
}

int main(void) {
    CyGlobalIntEnable;
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();

    toggle_semaphore = xSemaphoreCreateCounting(50, 0); // Increase count cap

    UART_PutString("Commands:\r\n");
    UART_PutString(" toggle, on, off, status, reset\r\n");
    UART_PutString(" auto on, auto off, sleep, normal\r\n");

    if(toggle_semaphore == NULL) {
        UART_PutString("Semaphore creation failed\r\n");
        while(1);
    }

    xTaskCreate(uart_input_task, "UART Task", 300, NULL, 3, NULL);
    xTaskCreate(led_task, "LED Task", 200, NULL, 2, NULL);
    xTaskCreate(auto_event_task, "Auto Task", 200, NULL, 1, NULL);

    vTaskStartScheduler();
    for(;;) {}
}
```

Note: All the header, C files as well as `FreeRTOS.c` and `FreeRTOSConfig.h` need to be included for all the projects implemented through the FreeRTOS.

# Purpose of Counting Semaphore

In the above code example, counting semaphore helps in keeping the track of all the events even if they appear in quickly as per the user input. For instance, even if user manage to enter 10 events quickly, the counting semaphore will take them up one by one. Also, there can be multiple instances of the same event and the count is maintained as to how many times an event is occurred and not who caused the event.

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

<img width="530" height="555" alt="image" src="https://github.com/user-attachments/assets/d7c830f5-192f-4806-a297-206887beb815" />

<img width="482" height="920" alt="image" src="https://github.com/user-attachments/assets/fd028a9f-aaaf-4752-a0f3-f660caa89fb2" />

**— END**
