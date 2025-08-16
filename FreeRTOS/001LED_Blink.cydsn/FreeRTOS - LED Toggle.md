# FreeRTOS - LED Toggle

# Objective

To toggle the LED on the PSoC 5 board but through the `xTaskCreate()` function in FreeRTOS and also display the respective string through UART on Tera Term.

# Working

In this lab exercise, one task named `led_task()` is created using `xTaskCreate()` function in which the LED pin is toggled infinitely under the `while(1)` super loop. Also within this while loop, there is an if condition defined on led state which prints the string as per the appropriate state of the LED i.e., LED ON or LED OFF.

# PSoC-Schematic

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%201.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%202.png)

# Pin Assignment

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%203.png)

Here Pin P2[1] is fixed on the PSoC board itself for LED. In case you want to use any external LED or display unit, assign the appropriate GPIO for the same.

# PSoC-Code

```c
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
```

Note: All the header, C files as well as `FreeRTOS.c` and `FreeRTOSConfig.h` need to be included for all the projects implemented through the FreeRTOS.

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

![image.png](Addition%20without%20Pointers%20206d051c359e80278902dfcca222e0eb/image%203.png)

![image.png](FreeRTOS%20-%20LED%20Toggle%20231d051c359e805b9524f77a2f97c222/image.png)

**— END**