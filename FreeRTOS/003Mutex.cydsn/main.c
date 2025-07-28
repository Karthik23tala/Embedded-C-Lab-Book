#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

extern void FreeRTOS_Start();

#define CMD_OFF     1
#define CMD_ON      2
#define CMD_BLINK   3
#define CMD_BREATHE 4

SemaphoreHandle_t led_mutex;
QueueHandle_t led_cmd_queue;
uint8_t current_cmd = 0;

void uart_input_task(void *ptr) {
    char buffer[10];
    uint8_t index = 0;

    while (1) {
        char ch = UART_GetChar();
        if (ch != 0u && ch != 0xFF) {
            if (ch == '\r' || ch == '\n') {
                buffer[index] = '\0';
                index = 0;
                uint8_t cmd = atoi(buffer);

                if (cmd >= 1 && cmd <= 4) {
                    xQueueSend(led_cmd_queue, &cmd, portMAX_DELAY);
                } else {
                    UART_PutString("\r\nInvalid input. Use 1 to 4.\r\n");
                }
            } else if (index < sizeof(buffer) - 1) {
                buffer[index++] = ch;
                UART_PutChar(ch);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void led_control_task(void *ptr) {
    uint8_t cmd;
    while (1) {
        if (xQueueReceive(led_cmd_queue, &cmd, portMAX_DELAY) == pdPASS) {
            current_cmd = cmd;
            if (xSemaphoreTake(led_mutex, portMAX_DELAY)) {
                switch (cmd) {
                    case CMD_ON:
                        PWM_Stop();
                        led_Write(0); // LED ON (active-low)
                        UART_PutString("\r\nLED OFF\r\n");
                        break;
                    case CMD_OFF:
                        PWM_Stop();
                        led_Write(1); // LED OFF (active-low)
                        UART_PutString("\r\nLED ON\r\n");
                        break;
                    case CMD_BLINK:
                        UART_PutString("\r\nBlink mode enabled\r\n");
                        break;
                    case CMD_BREATHE:
                        UART_PutString("\r\nPulse brightness mode enabled\r\n");
                        break;
                }
                xSemaphoreGive(led_mutex);
            }
        }
    }
}

void blink_task(void *ptr) {
    while (1) {
        if (current_cmd == CMD_BLINK) {
            if (xSemaphoreTake(led_mutex, portMAX_DELAY)) {
                led_Write(!led_Read()); // Toggle LED
                xSemaphoreGive(led_mutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void breathe_task(void *ptr) {
    uint8_t brightness = 0;
    int8_t step = 5;

    while (1) {
        if (current_cmd == CMD_BREATHE) {
            if (xSemaphoreTake(led_mutex, portMAX_DELAY)) {
                led_Write(0); // LED ON
                xSemaphoreGive(led_mutex);
                vTaskDelay(pdMS_TO_TICKS(brightness));

                if (xSemaphoreTake(led_mutex, portMAX_DELAY)) {
                    led_Write(1); // LED OFF
                    xSemaphoreGive(led_mutex);
                }
                vTaskDelay(pdMS_TO_TICKS(100 - brightness));

                brightness += step;
                if (brightness >= 100 || brightness <= 0) step = -step;
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}

int main(void)
{
    CyGlobalIntEnable;
    FreeRTOS_Start();

    UART_Start();
    UART_ClearTxBuffer();
    UART_PutString("FreeRTOS system starting...\r\n");
    UART_PutString("Enter 1=ON, 2=OFF, 3=BLINK, 4=PULSE\r\n");

    PWM_Start();
    PWM_WriteCompare(0);
    PWM_Stop();

    led_Write(0); // LED OFF initially (active-low)

    led_mutex = xSemaphoreCreateMutex();
    led_cmd_queue = xQueueCreate(5, sizeof(uint8_t));

    if (led_mutex == NULL || led_cmd_queue == NULL) {
        UART_PutString("Failed to create RTOS objects\r\n");
        while (1);
    }

    xTaskCreate(uart_input_task, "UART Task", 256, NULL, 3, NULL);
    xTaskCreate(led_control_task, "LED Task", 256, NULL, 2, NULL);
    xTaskCreate(blink_task, "Blink Task", 200, NULL, 1, NULL);
    xTaskCreate(breathe_task, "Breathe Task", 200, NULL, 1, NULL);

    UART_PutString("RTOS Scheduler starting...\r\n");
    vTaskStartScheduler();

    for (;;);
}