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