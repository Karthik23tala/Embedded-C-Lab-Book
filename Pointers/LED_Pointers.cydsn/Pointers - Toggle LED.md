# Pointers - Toggle LED

# Objective

To toggle the LED on the PSoC 5 board based on the string entered by the user. 

# Working

In this exercise, LED on the PSoC board is toggled with the command from the user taken in string format. For instance, “LED OFF” and “LED ON”, these commands if entered by user in Tera Term then LED will toggle accordingly. The text entered by user is stored in one string named input whereas the required string is defined already. Now the input is compared with the defined strings, LED OFF and LED ON using strncmp() function. If there is a match, then the Pin_LED is written 1 or 0 thereby turning the LED off or on accordingly. Along with the LED toggling, it displays the respective message on the tera term. This message is stored in the string named response.

# PSoC-Schematic

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%201.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%202.png)

# Pin Assignment

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%203.png)

Here Pin P2[1] is fixed on the PSoC board itself for LED. In case you want to use any external LED or display unit, assign the appropriate GPIO for the same.

# PSoC-Code

```c
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
```

# Results (Tera Term)

To view the results in Tera Term, set the baud rate to 115200 bps in Tera Term -> Serial -> Serial Port -> Speed.

Note: Change the baud rate to the same used in the schematic for every lab wherever UART component is used in the Top Design.

![image.png](Addition%20without%20Pointers%20206d051c359e80278902dfcca222e0eb/image%203.png)

![image.png](Pointers%20-%20Toggle%20LED%20206d051c359e803494fae699c464df0c/image%204.png)

**— END**