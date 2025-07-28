#include "project.h" // Project header file
#include <stdio.h> // Header file to use standard input and output functions
#include <string.h> // Header file to use string functions
void led_on() {
    UART_PutString("You selected Option 1: LED ON\r\n");
    Pin_LED_Write(1);
}
void led_off() {
    UART_PutString("You selected Option 2: LED OFF\r\n");
    Pin_LED_Write(0);
}
void led_blink() {
    UART_PutString("You selected Option 3: Blink LED\r\n");
    for (int i = 0; i < 5; i++) {
        Pin_LED_Write(1);
        CyDelay(300);
        Pin_LED_Write(0);
        CyDelay(300);
    }
}
typedef void (*MenuFunction)();
void printMenu() {
    UART_PutString("\r\n===== MENU =====\r\n");
    UART_PutString("1. Turn LED ON\r\n");
    UART_PutString("2. Turn LED OFF\r\n");
    UART_PutString("3. Blink LED\r\n");
    UART_PutString("Enter your choice (1-3): ");
}

int main(void)
{
    CyGlobalIntEnable;
    UART_Start();
    UART_PutString("Function Pointer Example with Menu\r\n");
    MenuFunction menu[] = {led_on,led_off,led_blink}; 
    //More options can be added here as per the function definition at the start of the code
    char input;
    for(;;)
    {
        printMenu();

        while (UART_GetRxBufferSize() == 0); // wait for input
        input = UART_GetChar();
        UART_PutChar(input); // echo
        UART_PutString("\r\n");

        if (input >= '1' && input <= '3') {
            uint8_t index = input - '1';
            menu[index]();
        } else {
            UART_PutString("Invalid input! Please enter 1, 2, or 3.\r\n");
        }
    }
}