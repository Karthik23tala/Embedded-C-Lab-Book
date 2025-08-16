# Function Pointer - LED Menu

# Objective

To use function pointer to display the LED menu with options for led ON, OFF and BLINK and thereby perform respective operation on the PSoC 5 board. 

# Working

**In this lab exercise we used function pointers** in Embedded C on a PSoC 5LP board to create a menu-driven LED control system via **UART**. Firstly defined three user functions namely `led_on()`, `led_off()`, and `led_blink()`which will control the LED and stores their addresses in a function pointer array. The user inputs the number through **Tera Term**, where the menu is printed using `UART_PutString()`, and the user's choice (1–3) is read using `UART_GetChar()`. With respect to the input provided, the respective function is called through the function pointer, and the LED responds accordingly.

# PSoC-Schematic

<img width="865" height="516" alt="image" src="https://github.com/user-attachments/assets/1989073b-75e4-47a1-ac01-9233f85140a4" />

<img width="900" height="693" alt="image" src="https://github.com/user-attachments/assets/d108b3e9-50a4-4c2b-851a-d38a2a3a04ca" />

<img width="865" height="661" alt="image" src="https://github.com/user-attachments/assets/005e0ed7-787f-4edc-b556-ed56893e962c" />

# Pin Assignment

<img width="775" height="283" alt="image" src="https://github.com/user-attachments/assets/b95e49dc-7b65-4744-ad6e-9a9688d0e648" />

Here Pin P2[1] is fixed on the PSoC board itself for LED. In case you want to use any external LED or display unit, assign the appropriate GPIO for the same.

# PSoC Code

```c
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
//Function to take the user input and thereby pass the value to respective function pointer
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
    for(;;) //Runs infinitely for the input
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
```

# Purpose

In the above code example, the same functionality of LED menu can be implemented using a simple if else ladder but the advantage and purpose of using function pointers instead of if else statements is that the former makes the code more flexible, extendable and readable. For instance, if in further application we need to add a couple of more options such as breathing led or led pulse, etc. then it can simply be done by defining the specific user defined function at the top and adding these new options into the array. Also, the execution is faster in the function pointer method as opposed to if else because in this it can quickly jump to the user input number and call the respective function.

# Results (Tera Term)

<img width="413" height="692" alt="image" src="https://github.com/user-attachments/assets/79aebe5a-7054-43b0-b491-51ae752f50b8" />

**— END**
