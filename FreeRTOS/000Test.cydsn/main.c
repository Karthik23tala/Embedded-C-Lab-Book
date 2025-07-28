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

extern void     FreeRTOS_Start();

void uart_task(void *ptr){
    while(1){
        UART_PutString("this is a test\r\n");
        vTaskDelay(1000);
    }
}

void led_task(void *ptr){
    while(1){   
        led_Write( !led_Read() );
        vTaskDelay(1000);
    }
}


int main(void)
{   

    CyGlobalIntEnable; /* Enable global interrupts. */
    FreeRTOS_Start();
    UART_Start();
    UART_ClearTxBuffer();
    xTaskCreate(uart_task,"task1", 200, NULL, 5, NULL);  
    xTaskCreate(led_task,"task2", 200, NULL, 5, NULL);  
    vTaskStartScheduler();
    for(;;);
}