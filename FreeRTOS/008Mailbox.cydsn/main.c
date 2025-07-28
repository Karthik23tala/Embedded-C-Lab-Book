#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern void FreeRTOS_Start();

QueueHandle_t led_mailbox;
SemaphoreHandle_t uart_mutex;

typedef enum {
    LED_ON,
    LED_OFF,
    LED_TOGGLE,
    LED_STATUS,
    LED_BLINK,
    LED_SLEEP,
    LED_HELP,
    LED_INVALID
} led_cmd_t;

void uart_print(const char *msg) {
    if (xSemaphoreTake(uart_mutex, portMAX_DELAY)) {
        UART_PutString(msg);
        xSemaphoreGive(uart_mutex);
    }
}

void uart_putchar(char ch) {
    if (xSemaphoreTake(uart_mutex, portMAX_DELAY)) {
        UART_PutChar(ch);
        xSemaphoreGive(uart_mutex);
    }
}

void print_help() {
    uart_print(
        "\r\nAvailable Commands:\r\n"
        "on     - Turn LED ON\r\n"
        "off    - Turn LED OFF\r\n"
        "toggle - Toggle LED state\r\n"
        "status - Show LED state\r\n"
        "blink  - Blink LED 3 times\r\n"
        "sleep  - Pause for 5 seconds\r\n"
        "help   - Show this help message\r\n"
        "> "
    );
}

led_cmd_t parse_command(const char *input) {
    if (strcasecmp(input, "on") == 0) return LED_ON;
    if (strcasecmp(input, "off") == 0) return LED_OFF;
    if (strcasecmp(input, "toggle") == 0) return LED_TOGGLE;
    if (strcasecmp(input, "status") == 0) return LED_STATUS;
    if (strcasecmp(input, "blink") == 0) return LED_BLINK;
    if (strcasecmp(input, "sleep") == 0) return LED_SLEEP;
    if (strcasecmp(input, "help") == 0) return LED_HELP;
    return LED_INVALID;
}

void uart_input_task(void *ptr) {
    char buffer[32];
    uint8_t index = 0;

    while (1) {
        char ch = UART_GetChar();
        if (ch != 0u && ch != 0xFF) {
            if (ch == '\r' || ch == '\n') {
                buffer[index] = '\0';
                index = 0;

                led_cmd_t cmd = parse_command(buffer);
                if (cmd != LED_INVALID) {
                    xQueueOverwrite(led_mailbox, &cmd);
                    uart_print("\r\nCommand received\r\n");
                } else {
                    uart_print("\r\nInvalid command. Type 'help'\r\n");
                }
                uart_print("> ");
            } else if (index < sizeof(buffer) - 1) {
                buffer[index++] = ch;
                uart_putchar(ch);
            }
        }

        vTaskDelay(10);
    }
}

void led_task(void *ptr) {
    led_cmd_t cmd;

    while (1) {
        if (xQueueReceive(led_mailbox, &cmd, portMAX_DELAY) == pdTRUE) {
            switch (cmd) {
                case LED_ON:
                    Pin_LED_Write(1);
                    uart_print("LED ON\r\n");
                    break;
                case LED_OFF:
                    Pin_LED_Write(0);
                    uart_print("LED OFF\r\n");
                    break;
                case LED_TOGGLE:
                    Pin_LED_Write(!Pin_LED_Read());
                    uart_print(Pin_LED_Read() ? "LED ON\r\n" : "LED OFF\r\n");
                    break;
                case LED_STATUS:
                    uart_print(Pin_LED_Read() ? "LED is currently ON\r\n" : "LED is currently OFF\r\n");
                    break;
                case LED_BLINK:
                    for (int i = 0; i < 3; ++i) {
                        Pin_LED_Write(1);
                        vTaskDelay(pdMS_TO_TICKS(300));
                        Pin_LED_Write(0);
                        vTaskDelay(pdMS_TO_TICKS(300));
                    }
                    uart_print("Blinked 3 times\r\n");
                    break;
                case LED_SLEEP:
                    uart_print("Sleeping for 5 seconds...\r\n");
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    uart_print("Woke up\r\n");
                    break;
                case LED_HELP:
                    print_help();
                    break;
                default:
                    break;
            }
        }
    }
}

int main(void) {
    CyGlobalIntEnable;
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();

    uart_mutex = xSemaphoreCreateMutex();
    if (uart_mutex == NULL) {
        while (1); // Error creating mutex
    }

    led_mailbox = xQueueCreate(1, sizeof(led_cmd_t));
    if (led_mailbox == NULL) {
        uart_print("Failed to create mailbox\r\n");
        while (1);
    }

    print_help();

    xTaskCreate(uart_input_task, "UART Task", 200, NULL, 2, NULL);
    xTaskCreate(led_task, "LED Task", 200, NULL, 2, NULL);

    vTaskStartScheduler();
    for (;;) {}
}