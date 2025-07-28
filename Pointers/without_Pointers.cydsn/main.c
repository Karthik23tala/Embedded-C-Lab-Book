#include <project.h>
#include <stdio.h>

int addNumbers(int a, int b) {
    return a + b;
}

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

int main() {
    CyGlobalIntEnable;
    UART_Start();

    int num1 = 0, num2 = 0, sum = 0;
    char buffer[50];

    UART_PutString("Press 'q' to quit at any time.\r\n\r\n");

    while (1) {
        UART_PutString("Enter first number: ");
        UART_ReadLine(buffer, sizeof(buffer));
        if (buffer[0] == 'q' || buffer[0] == 'Q') break;
        sscanf(buffer, "%d", &num1);

        UART_PutString("Enter second number: ");
        UART_ReadLine(buffer, sizeof(buffer));
        if (buffer[0] == 'q' || buffer[0] == 'Q') break;
        sscanf(buffer, "%d", &num2);

        sum = addNumbers(num1, num2);
        sprintf(buffer, "Sum: %d\r\n\r\n", sum);
        UART_PutString(buffer);
    }

    UART_PutString("Program terminated. You can now close the window.\r\n");

    return 0;
}