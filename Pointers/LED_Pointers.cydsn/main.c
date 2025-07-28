#include <project.h>
#include <string.h>
#include <stdio.h>

void parseCommand(char *input, char *response) {
    if (strncmp(input, "LED ON", 6) == 0) {
        Pin_LED_Write(1);
        strcpy(response, "LED turned ON\r\n");
    } else if (strncmp(input, "LED OFF", 7) == 0) {
        Pin_LED_Write(0);
        strcpy(response, "LED turned OFF\r\n");
    } else {
        strcpy(response, "Unknown command\r\n");
    }
}

// Reads a string from UART with echo
void UART_ReadLine(char *buffer, int maxSize) {
    int i = 0;
    char ch;

    while (1) {
        if (UART_GetRxBufferSize() > 0) {
            ch = UART_GetChar();
            if (ch == '\r' || ch == '\n') {
                buffer[i] = '\0';
                UART_PutString("\r\n");
                break;
            }
            UART_PutChar(ch);
            buffer[i++] = ch;
            if (i >= maxSize - 1) break;
        }
    }
}

int main(void) {
    CyGlobalIntEnable;
    UART_Start();

    UART_PutString("UART Command Parser Ready\r\n");
    UART_PutString("Type 'LED ON' or 'LED OFF'\r\n\r\n");

    char input[50];
    char response[50];

    for (;;) {
        UART_PutString("> ");
        UART_ReadLine(input, sizeof(input));
        parseCommand(input, response);
        UART_PutString(response);
    }
}