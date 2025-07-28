#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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