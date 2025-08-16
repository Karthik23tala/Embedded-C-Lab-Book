# Addition without Pointers

# Objective

To add two numbers using a user defined function and return the result to the main function thereby the same on Tera Term through UART Communication Protocol.

# Working

In this exercise, two numbers a and b are added using the `addNumbers()` function. These two numbers are stored in the buffer with the size of 50 as defined in the code. `UART_ReadLine()` function is used to display the number entered by the user. CyGlobalIntEnable is used to enable the global interrupts in the program whereas `UART_Start()` starts the UART block in the top design. The while loop keeps the program running infinitely and termination is done when the user enters q or Q in either of the input numbers

# PSoC-Schematic

<img width="779" height="488" alt="image" src="https://github.com/user-attachments/assets/a36954a7-8517-4eb5-bc35-2e13800fbc5a" />

<img width="900" height="693" alt="image" src="https://github.com/user-attachments/assets/d397725a-34dd-4253-b7eb-b11673ae3379" />

# Pin Assignment

<img width="727" height="226" alt="image" src="https://github.com/user-attachments/assets/0bf9dfcf-038e-481d-9eae-ad707db8cd29" />

The Pin assignment of Rx and Tx pins remain fixed for PSoC 5 development board. In case you are using any other development, refer the datasheet.

# PSoC-Code

```c
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
```

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

<img width="530" height="555" alt="image" src="https://github.com/user-attachments/assets/dc1d4818-1ed0-4543-bff3-1448b7b589bd" />

<img width="900" height="367" alt="image" src="https://github.com/user-attachments/assets/c5df96b9-c3e3-4f5a-8341-c657345d098f" />

**— END**
