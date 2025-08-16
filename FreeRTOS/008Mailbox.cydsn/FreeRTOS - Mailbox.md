# FreeRTOS - Mailbox

# Objective

To operate LED menu with multiple options for user to enter and thereby control the onboard LED with the help of Mailbox. This lab exercise will help understand the working of Mailbox and also the difference between Queue and Mailbox in an RTOS system.

# Mailbox - Definition

In an RTOS system like FreeRTOS, Mailbox is used to a mailbox is used to send the latest message or command from one task to another, using a queue of length 1, so that the receiver always processes the most recent information without buffering old messages. Since, the mailbox has fixed single slot unlike queue, the latest value is always overwritten into the slot available with the current value.

# Difference between Queue and Mailbox

The primary difference between the working of Queue and Mailbox in an RTOS system is that Queue can hold multiple data values of user defined length whereas Mailbox can hold only one data value. Due to this, unlike Queue which works in FIFO order, in Mailbox a new value always overwrites the current value. Also, if Queue is full then it blocks writing of new values but for Mailbox always the latest value is written in the single slot.

# Working

In the below lab exercise, we have designed an LED menu system where the user enters one of the required commands in tera term window to perform the respective action. For instance, ON will turn the led on, OFF will turn the led off, STATUS will display the status of the led, BLINK will blink the led 3 times, SLEEP will make the led sleep for 5 seconds and HELP will list down the commands and their respective actions. To store the user entered strings we have used queue of size 5 meaning at max 5 commands can be stored. For every appropriate, the tera term window shows the proper output. In case user input matches with none of the pre-defined commands, then the UART prints “Invalid command”.

# PSoC-Schematic

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%201.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%202.png)

# Pin Assignment

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%203.png)

Here Pin P2[1] is fixed on the PSoC board itself for LED. In case you want to use any external LED or display unit, assign the appropriate GPIO for the same.

# PSoC-Code - with Mailbox

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern void FreeRTOS_Start();

QueueHandle_t led_mailbox;

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

void print_help() {
    UART_PutString(
        "\r\nAvailable Commands:\r\n"
        "on     - Turn LED ON\r\n"
        "off    - Turn LED OFF\r\n"
        "toggle - Toggle LED state\r\n"
        "status - Show LED state\r\n"
        "blink  - Blink LED 3 times\r\n"
        "sleep  - Pause for 5 seconds\r\n"
        "help   - Show this help message\r\n"
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
                    UART_PutString("\r\nCommand received\r\n");
                } else {
                    UART_PutString("\r\nInvalid command. Type 'help'\r\n");
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
    led_cmd_t cmd;

    while (1) {
        if (xQueueReceive(led_mailbox, &cmd, portMAX_DELAY) == pdTRUE) {
            switch (cmd) {
                case LED_ON:
                    led_Write(1);
                    UART_PutString("LED ON\r\n");
                    break;
                case LED_OFF:
                    led_Write(0);
                    UART_PutString("LED OFF\r\n");
                    break;
                case LED_TOGGLE:
                    led_Write(!led_Read());
                    UART_PutString(led_Read() ? "LED ON\r\n" : "LED OFF\r\n");
                    break;
                case LED_STATUS:
                    UART_PutString(led_Read() ? "LED is currently ON\r\n" : "LED is currently OFF\r\n");
                    break;
                case LED_BLINK:
                    for (int i = 0; i < 3; ++i) {
                        led_Write(1);
                        vTaskDelay(pdMS_TO_TICKS(300));
                        led_Write(0);
                        vTaskDelay(pdMS_TO_TICKS(300));
                    }
                    UART_PutString("Blinked 3 times\r\n");
                    break;
                case LED_SLEEP:
                    UART_PutString("Sleeping for 5 seconds...\r\n");
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    UART_PutString("Woke up\r\n");
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

    print_help();

    led_mailbox = xQueueCreate(1, sizeof(led_cmd_t));
    if (led_mailbox == NULL) {
        UART_PutString("Failed to create mailbox\r\n");
        while (1);
    }

    xTaskCreate(uart_input_task, "UART Task", 200, NULL, 2, NULL);
    xTaskCreate(led_task, "LED Task", 200, NULL, 2, NULL);

    vTaskStartScheduler();
    for (;;) {}
}

```

Note: All the header, C files as well as `FreeRTOS.c` and `FreeRTOSConfig.h` need to be included for all the projects implemented through the FreeRTOS.

# Output before using mutex

![image.png](FreeRTOS%20-%20Mailbox%2023ad051c359e800dacb7dd02b49ea76c/image.png)

For the above code without any mutex we can see that the initial menu is shown correctly and also once the user enters any relevant string the action is performed appropriately but the statements are printed in jumbled order such as `"Command receiLED OFF"`. This is because while printing any statement onto Tera Term through UART two tasks are concurrently trying to access the UART while using the functions such as `UART_PutString()` and `UART_PutChar()`. 

# Purpose of Mutex in the above code

The above jumble of output statements in Tera Term can be fixed by using Mutex for UART printing functions. Instead of the in-built UART functions such as  `UART_PutString()` and `UART_PutChar()` we have used user defined functions `uart_print()` and `uart_putchar()` where a mutex called `uart_mutex` is used. The purpose of this defined mutex is that whenever any statement needs to be printed and showed on Tera Term, the function will take the mutex, print the entire message and then give the mutex back. By this, it eliminates any concurrency or jumbling of UART access and we get properly formatted output.

# PSoC-Code - with Mailbox and Mutex

```c
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
```

# Results (Tera Term) - Proper Output

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

As mentioned above, Mailbox overwrites the current value with latest value. In the experiment, we have entered SLEEP first which takes the LED to sleep mode for 5 seconds. Now followed by this, we have entered ON and OFF in the same order. We can also see there is a `command received` statement for both the user entered commands. But once, the LED is woken up, it turns LED OFF only because the ON command is overwritten with OFF command in the Mailbox. The same can be seen the output window in Tera Term.

![image.png](Addition%20without%20Pointers%20206d051c359e80278902dfcca222e0eb/image%203.png)

![image.png](FreeRTOS%20-%20Mailbox%2023ad051c359e800dacb7dd02b49ea76c/image%201.png)

**— END**