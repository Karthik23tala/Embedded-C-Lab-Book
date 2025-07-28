#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

extern void FreeRTOS_Start();

void led_task(void *ptr){
    while(1){
        uint8_t led_state = !led_Read();  // Toggle state
        led_Write(led_state);             // Write to LED pin
        if(led_state)
            UART_PutString("LED ON\r\n");
        else
            UART_PutString("LED OFF\r\n");
        vTaskDelay(1000); // Delay 1 second (1000 ticks)
    }
}

int main(void)
{
    CyGlobalIntEnable;
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();
    xTaskCreate(led_task, "LED_UART_Task", 200, NULL, 5, NULL);
    vTaskStartScheduler();

    for(;;) { }
}