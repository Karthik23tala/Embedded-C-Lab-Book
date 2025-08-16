# Addition with Pointers

# Objective

To add two numbers using a user defined function and return the result through the pointers unlike the previous example to the main function thereby the same on Tera Term through UART Communication Protocol.

# Working

In this exercise, two numbers a and b are added using the `addNumbers()` function but the only difference being is that in this case the function has three arguments namely *a, *b and *result which are pointer variables. Now, when the addition is to be performed, the address of num1, num2 and sum are passed in order to perform the required addition and return the result into the ‘sum’ variable. Similar to the previous case, these two numbers are stored in the buffer with the size of 50 as defined in the code. `UART_ReadLine()` function is used to display the number entered by the user. CyGlobalIntEnable is used to enable the global interrupts in the program whereas `UART_Start()` starts the UART block in the top design. The while loop keeps the program running infinitely and termination is done when the user enters q or Q in either of the input numbers. 

# PSoC-Schematic

![image.png](Addition%20without%20Pointers%20206d051c359e80278902dfcca222e0eb/image.png)

![image.png](Addition%20without%20Pointers%20206d051c359e80278902dfcca222e0eb/image%201.png)

# PSoC-Code

```c
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
```

# Purpose

In the provided code, the `addNumbers()` function uses pointers to perform addition, but the same result could be achieved without pointers by returning the sum directly. However, using pointers (pass by reference) allows the function to modify the caller's variable directly, eliminating the need to return a value and enabling potential modifications to multiple variables whereas passing regular variables (pass by value) copies the data, which is less efficient for large data types like structures or arrays due to the overhead of copying. While pointers are somewhat overkill for adding two integers in this example, they significantly improve efficiency for larger data structures by passing only memory addresses, reducing copying overhead.

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

![image.png](Addition%20without%20Pointers%20206d051c359e80278902dfcca222e0eb/image%203.png)

![image.png](Addition%20with%20Pointers%20206d051c359e80d08bd3edfe89fe52a8/image.png)

**— END**