# memcpy() - String Matching

# Objective

To use `memcpy()` function in C programming to copy the string from one variable to another variable

# Working

**In this lab exercise we used `memcpy()` function** in Embedded C on a PSoC 5LP board to compare the user entered string and thereby toggle the LED as per the comparison result. Specifically in the code, we simply asked the user to enter some string of length less than 63 characters and stored it into variable `intBuffer` . Now using `memcpy()` function, we have copied this string to another variable named `copyBuffer` . After copying this string, we have compared both of the variables using `strcmp()` function. This function returns the value as true or false. If true, then the LED will be turned on for 2 seconds but if false, then it will stay turned OFF itself.

# PSoC-Schematic

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%201.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%202.png)

# Pin Assignment

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%203.png)

Here Pin P2[1] is fixed on the PSoC board itself for LED. In case you want to use any external LED or display unit, assign the appropriate GPIO for the same.

# PSoC Code

```c
#include "project.h"
#include <string.h>
#include <stdio.h>
#define BUFFER_SIZE 64
char inputBuffer[BUFFER_SIZE];
char copyBuffer[BUFFER_SIZE];
char txBuffer[128];

void initHardware() {
    UART_Start();
    Pin_LED_Write(0); // LED OFF initially
    CyGlobalIntEnable;
}

void readUserInput() {
    uint8_t index = 0;
    char ch;
    while (index < BUFFER_SIZE - 1) {
        while (!UART_GetRxBufferSize()); // Wait for input
        ch = UART_GetChar();
        if (ch == '\r' || ch == '\n') {
            break;
        }
        inputBuffer[index++] = ch;
        UART_PutChar(ch); // Echo
    }
    inputBuffer[index] = '\0';
    UART_PutString("\r\n");
}

int main(void) {
    initHardware();
    UART_PutString("PSoC memcpy() Demo with LED and UART\r\n");
    while (1) {
				UART_PutString("Enter a string (max 63 chars): ");
        readUserInput();
        memcpy(copyBuffer, inputBuffer, strlen(inputBuffer) + 1);
        if (strcmp(inputBuffer, copyBuffer) == 0) {
            UART_PutString("Copy successful. Strings match.\r\n");
            Pin_LED_Write(1); // LED ON
        } else {
            UART_PutString("Copy failed. Strings do not match.\r\n");
            Pin_LED_Write(0); // LED OFF
        }
        sprintf(txBuffer, "Original: %s\r\nCopy: %s\r\n\r\n", inputBuffer, copyBuffer);
        UART_PutString(txBuffer);
        CyDelay(2000); // Keep the LED ON for 2 seconds
        Pin_LED_Write(0); // Turn OFF LED after delay
    }
}
```

# Purpose

The primary question that may arise is that why not simply define as `copyBuffer = intBuffer` but that does not work in C as string is nothing but an array of characters. So it will result in error. Another alternative is to use `strcpy()` function which works to some extend but there are two drawbacks. It works only with strings i.e., characters and secondly it will terminate the copying after encountering NULL character. So, `memcpy()` is used in this example in which it copies the entire string. From the output we can observe that there are spaces, numbers, decimals, etc. in the user entered string and it works every time.

# Results (Tera Term)

![image.png](memcpy()%20-%20String%20Matching%20216d051c359e80f6a569e0c04c55d31b/image.png)

**— END**