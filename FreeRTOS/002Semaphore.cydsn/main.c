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
        if(ch != 0u && ch != 0xFF) { // Valid character
            if(ch == '\r' || ch == '\n') { // End of line
                buffer[index] = '\0';
                index = 0;
                if( (strcmp(buffer, "toggle") == 0) || (strcmp(buffer, "Toggle") == 0) || (strcmp(buffer, "TOGGLE") == 0) ) {
                    UART_PutString("\r\nToggle command received\r\n");
                    xSemaphoreGive(led_semaphore); // Give semaphore to LED task
                } else {
                    UART_PutString("\r\nUnknown command\r\n");
                }
            } else if(index < sizeof(buffer) - 1) {
                buffer[index++] = ch;
                UART_PutChar(ch); // Echo back to user
            }
        }
        vTaskDelay(10); // Small delay to allow context switch
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

int main(void)
{
    CyGlobalIntEnable;
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();
    led_semaphore = xSemaphoreCreateBinary();
    if(led_semaphore == NULL) {
        UART_PutString("Failed to create semaphore\r\n");
        while(1);
    }
    xTaskCreate(uart_input_task, "UART Task", 200, NULL, 3, NULL);
    xTaskCreate(led_task, "LED Task", 200, NULL, 2, NULL);
    vTaskStartScheduler();
    for(;;) { }
}