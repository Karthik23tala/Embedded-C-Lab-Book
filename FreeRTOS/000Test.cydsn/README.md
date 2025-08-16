# How to install FreeRTOS on PSoC 5 Development Board

# Objective

This lab demonstrates step by step guide on how to install FreeRTOS on the PSoC 5 development board. Also, after the proper installation of FreeRTOS kernel files and integration with the PSoC 5 board, we will also implement a small task to ensure the proper installation.

# Step by Step Guide

- Visit [www.freertos.org](http://www.freertos.org) and download the FreeRTOS from the homepage.
- Create a PSoC project in the PSoC Creator app and select CY8C5888LTI-LP097 board from the list of PSoC 5.
- In the next step, create the folders inside the PSoC project folder in the below manner.

```c
/FreeRTOS/Source
/FreeRTOS/Source/include
/FreeRTOS/Source/gcc/ARM_CM3 /* As PSoC 5 board is based on ARM Cortex M3 Processor*/
/FreeRTOS/Source/gcc/MemMang
```

- Now copy the contents from FreeRTOS to the FreeRTOS folder created inside PSoC project folder respectively. For Memory Management, FreeRTOS provides five files namely heap_1, 2, 3, 4 and - Out of the five files, we can use heap_1 for our basic application. So copy and paste the file accordingly.
- After this, for better organization we can create the folder structure for FreeRTOS files. To add new folder right click on the project → Add → New Folder.

<img width="839" height="294" alt="image" src="https://github.com/user-attachments/assets/a23466e7-7c13-4728-b3c6-1863f64079bd" />

- Post the folder creation, import the files into respective folder for FreeRTOS. To import the files, right click on the project → Add → Existing item. In the window opened, browse the files from the folder and click open.
- After importing all the necessary header files, C files and memory management heap files, the folder should look similar to the below.

<img width="304" height="326" alt="image 1" src="https://github.com/user-attachments/assets/15b786c9-11ef-4801-b32e-b27974ecd87e" />

- FreeRTOS provides all the required header and C files to run the program on RTOS using any development board such as PSoC 5. But there are two files that we need to create specifically for this board and they are namely, `FreeRTOS.c` and `FreeRTOSConfig.h` . To create new files, select new item in the above menu.

<img width="318" height="459" alt="image 2" src="https://github.com/user-attachments/assets/ca3291c8-b420-43e1-83d5-8588c2e695eb" />

- The code that needs to used for the files is mentioned in the below sections.
- At the end, some additional paths need to be added in the project settings. For this right click on the project → Build Settings → ARM GCC → Compiler → General. In the General menu, select the three dots on the side of additional include directories. In the window opened Add the path as mentioned below by browsing through the file explorer. Click Apply and OK.
- In the same build settings, navigate to ARM GCC → Linker → General, in additional libraries click and add 'm' and also in the bottom line use newlib-nano change it to True. Click apply and OK.

<img width="1590" height="968" alt="image 3" src="https://github.com/user-attachments/assets/28669e29-a021-4773-b349-61fc58bfee5e" />

<img width="531" height="475" alt="image 4" src="https://github.com/user-attachments/assets/dd1f411e-4491-4375-b5b4-db994361aa0c" />

Note: The above process remains the same for any development board used in PSoC Creator app. The only difference being that instead of the files from ARM_CM3, use the files from the respective processor folder provided in the FreeRTOS depending on the development board being used. For instance, PSoC 5 is based on ARM Cortex M3, hence we have used the files from `GCC/ARM_CM3` folder. 

# Code for FreeRTOS.c:

```c
#include <project.h>
#include "FreeRTOS.h"

extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);

#define CORTEX_INTERRUPT_BASE          (16)

void FreeRTOS_Start(){
    
    /* Handler for Cortex Supervisor Call (SVC, formerly SWI) - address 11 */
    CyIntSetSysVector( CORTEX_INTERRUPT_BASE + SVCall_IRQn,
        (cyisraddress)vPortSVCHandler );
    
    /* Handler for Cortex PendSV Call - address 14 */
	CyIntSetSysVector( CORTEX_INTERRUPT_BASE + PendSV_IRQn,
        (cyisraddress)xPortPendSVHandler );    
    
    /* Handler for Cortex SYSTICK - address 15 */
	CyIntSetSysVector( CORTEX_INTERRUPT_BASE + SysTick_IRQn,
        (cyisraddress)xPortSysTickHandler );
}
```

# Code for FreeRTOSConfig.h Header File:

```c
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#include <project.h>

//#define SYSTEM_SUPPORT_OS		    1
    
#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configMAX_PRIORITIES		( 6 )
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			( ( unsigned long ) BCLK__BUS_CLK__HZ )
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 100 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 32 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 12 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		0
#define configUSE_CO_ROUTINES 		0
#define configUSE_MUTEXES			1

#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

#define configUSE_COUNTING_SEMAPHORES 	1
#define configUSE_ALTERNATIVE_API 		0
#define configCHECK_FOR_STACK_OVERFLOW	0   //2
#define configUSE_RECURSIVE_MUTEXES		1
#define configQUEUE_REGISTRY_SIZE		10
#define configGENERATE_RUN_TIME_STATS	0
#define configUSE_MALLOC_FAILED_HOOK	0   //1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskCleanUpResources		0
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_uxTaskGetStackHighWaterMark	1
#define INCLUDE_eTaskGetState				1

/**
 * Configure the number of priority bits. This is normally
 * __NVIC_PRIO_BITS but PSoC Creator beta 5 contained a larger
 * value for the priority than is implemented in the hardware so
 * set it here to what the data sheet describes.
 */
#define configPRIO_BITS       3        /* 8 priority levels */

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY 	( 7 << (8 - configPRIO_BITS) )

/* Priority 5, or 160 as only the top three bits are implemented. */
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( 5 << (8 - configPRIO_BITS) )

#endif /* FREERTOS_CONFIG_H */
```

# PSoC-Schematic

<img width="896" height="537" alt="image 5" src="https://github.com/user-attachments/assets/992c15c7-fed6-4106-a51b-6d6d27854a78" />

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
```

In the above code, `timer.h`, `queue.h` and `semphr.h` are not required for the application being implemented but it is a good practice to include all the header files.

# Working

In the above code, we have created two tasks namely task1 and task2 using `xTaskCreate()` function. Among the two, task1 is used to transmit the data string through UART communication protocol. Similarly, there is task2 which is used to toggle the state of LED for every one second.

Note: Just to toggle the LED and print the message on UART, we need not use FreeRTOS on PSoC 5 board. Rather real time operating system is used to manage multiple tasks running on the single board, manage the shared resources and more complex applications. In the coming lab sessions, we will explore the concepts of RTOS such as semaphores and mutexes. 

# Results (in Tera Term)

<img width="438" height="231" alt="image 6" src="https://github.com/user-attachments/assets/f0990d72-f225-459a-ae45-82446e318a89" />

**— END**
