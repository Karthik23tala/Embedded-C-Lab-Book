#include <project.h>
#include <stdio.h>

void addNumbers(int *a, int *b, int *result) {
    *result = *a + *b;
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
            if (i < maxSize - 1) {
                buffer[i++] = ch;
            }
        }
    }
}

int main() {
    CyGlobalIntEnable;
    UART_Start();

    char buffer[50];
    int num1, num2, sum;
    
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

        addNumbers(&num1, &num2, &sum);
        sprintf(buffer, "Sum: %d\r\n", sum);
        UART_PutString(buffer);
    }

    UART_PutString("Program terminated. You can now close the window.\r\n");

    return 0;
}