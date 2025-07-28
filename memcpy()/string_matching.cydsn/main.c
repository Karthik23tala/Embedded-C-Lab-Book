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

void promptUser() {
    UART_PutString("Enter a string (max 63 chars): ");
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
        promptUser();
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

        CyDelay(2000);
        Pin_LED_Write(0); // Turn OFF LED after delay
    }
}