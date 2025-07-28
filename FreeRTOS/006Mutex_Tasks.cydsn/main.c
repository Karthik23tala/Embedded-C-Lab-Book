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