# FreeRTOS - Queue

# Objective

To operate LED menu with multiple options for user to enter and thereby control the onboard LED with the help of Queue in RTOS which will execute the commands in a FIFO manner.

# Queue - Definition

A queue in RTOS is used as a communication mechanism between multiple tasks or ISRs in the system whereby one task or ISR can send or receive the data without any help of shared variables or complicated synchronization. It can considered as a **FIFO (First-In, First-Out)** buffer used to hold fixed-size data items whereby the programmer defines the maximum size of the queue. The data is stored and executed in FIFO manner. Also another advantage of queue is that it is thread safe meaning multiple tasks or ISRs can access the data within the queue. As a simple analogy, a queue in RTOS can be considered as a conveyor belt in a manufacturing factory where one of the coworkers places the item at one end and the other worker picks them up for further processing at the another end in the order they have been placed. 

# Working

In the below lab exercise, we have designed an LED menu system where the user enters one of the required commands in tera term window to perform the respective action. For instance, ON will turn the led on, OFF will turn the led off, STATUS will display the status of the led, BLINK will blink the led 3 times, SLEEP will make the led sleep for 5 seconds and HELP will list down the commands and their respective actions. To store the user entered strings we have used queue of size 5 meaning at max 5 commands can be stored. For every appropriate, the tera term window shows the proper output. In case user input matches with none of the pre-defined commands, then the UART prints “Invalid command”.

# PSoC-Schematic

<img width="865" height="516" alt="image" src="https://github.com/user-attachments/assets/559a6917-d7e1-47b5-b770-f683b572a4d6" />

<img width="900" height="693" alt="image" src="https://github.com/user-attachments/assets/4e660462-7363-4879-a0af-b055f0f145e9" />

<img width="865" height="661" alt="image" src="https://github.com/user-attachments/assets/da8ffe00-85f9-483e-87bf-1d76f4e6ef5d" />

# Pin Assignment

<img width="775" height="283" alt="image" src="https://github.com/user-attachments/assets/d40e2063-984f-4574-b1bb-b41ef0af3538" />

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

QueueHandle_t cmd_queue;

void uart_input_task(void *ptr) {
    char buffer[20];
    uint8_t index = 0;
    while(1) {
        char ch = UART_GetChar();
        if (ch != 0u && ch != 0xFF) {
            if (ch == '\r' || ch == '\n') {
                buffer[index] = '\0';
                index = 0;
                char *cmd = pvPortMalloc(strlen(buffer) + 1);
                if (cmd != NULL) {
                    strcpy(cmd, buffer);
                    xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
                }
                UART_PutString("\r\n> ");
            } else if (index < sizeof(buffer) - 1) {
                buffer[index++] = ch;
                UART_PutChar(ch);
            }
        }
        vTaskDelay(10);
    }
}

void led_task(void *ptr) {
    char *cmd;
    while(1) {
        if (xQueueReceive(cmd_queue, &cmd, portMAX_DELAY) == pdTRUE) {
            if (strcasecmp(cmd, "on") == 0) {
                Pin_LED_Write(1);
                UART_PutString("\r\nLED turned ON\r\n");
            } else if (strcasecmp(cmd, "off") == 0) {
                Pin_LED_Write(0);
                UART_PutString("\r\nLED turned OFF\r\n");
            } else if (strcasecmp(cmd, "status") == 0) {
                UART_PutString(Pin_LED_Read() ? "\r\nLED is ON\r\n" : "\r\nLED is OFF\r\n");
            } else if (strcasecmp(cmd, "blink") == 0) {
                UART_PutString("\r\nBlinking LED...\r\n");
                for (int i = 0; i < 3; i++) {
                    Pin_LED_Write(1);
                    vTaskDelay(pdMS_TO_TICKS(300));
                    Pin_LED_Write(0);
                    vTaskDelay(pdMS_TO_TICKS(300));
                }
                UART_PutString("\r\nDone blinking\r\n");
            } else if (strcasecmp(cmd, "sleep") == 0) {
                Pin_LED_Write(0);
                UART_PutString("\r\nSleeping for 5 seconds...\r\n");
                vTaskDelay(pdMS_TO_TICKS(5000));
                UART_PutString("\r\nWoke up from sleep\r\n");
            } else if (strcasecmp(cmd, "help") == 0) {
                UART_PutString(
                    "\r\nAvailable Commands:\r\n"
                    "on     - Turn LED ON\r\n"
                    "off    - Turn LED OFF\r\n"
                    "status - Show LED state\r\n"
                    "blink  - Blink LED 3 times\r\n"
                    "sleep  - Pause for 5 seconds\r\n"
                    "help   - Show this help message\r\n"
                );
            } else {
                UART_PutString("\r\nInvalid command\r\n");
            }
            vPortFree(cmd);
        }
    }
}

int main(void) {
    CyGlobalIntEnable;
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();
    cmd_queue = xQueueCreate(5, sizeof(char *));
    UART_PutString("Enter on/off/status/blink/sleep/help to control the LED\r\n> ");
    if(cmd_queue == NULL) {
        UART_PutString("Failed to create queue\r\n");
        while(1);
    }
    xTaskCreate(uart_input_task, "UART Task", 200, NULL, 3, NULL);
    xTaskCreate(led_task, "LED Task", 200, NULL, 2, NULL);
    vTaskStartScheduler();
    for(;;) {}
}
```

Note: All the header, C files as well as `FreeRTOS.c` and `FreeRTOSConfig.h` need to be included for all the projects implemented through the FreeRTOS.

# Purpose of Queue

In order to signify the use case of queue in the above code example, we have entered the series of strings in the Tera Term starting with sleep. When the user entered SLEEP, the led went to sleep mode for 5 seconds and within these 5 seconds, we have entered series of commands and once the led is woke from sleep it executes the commands in the entered order. The same can be seen in the tera term output window in results section.

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

<img width="530" height="555" alt="image" src="https://github.com/user-attachments/assets/8652b9af-c54b-4ddc-a4cc-5a1b5003924f" />

<img width="661" height="778" alt="image" src="https://github.com/user-attachments/assets/875ca9ef-15b9-4e9c-9f03-ec6499ecd9fa" />

**— END**
