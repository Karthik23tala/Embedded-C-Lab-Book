# Structures - Sensors Data

# Objective

To display the data of three sensors measuring the temperature, pressure and humidity using structures data type thereby displaying this information using Tera Term through UART communication protocol.

# Working

In this code, structure named Sensor is defined with three parameters named name, value and the unit. Using this definition the data from three sensors is being read and stored accordingly. The specific sensors used in this exercise are to record temperature, pressure and humidity. But since this lab uses only PSoC 5 board as the hardware, for the values a random value generator getPseudoRandom() function is being used. In real application, one can connect the data pin of the appropriate sensor to one of the GPIO pins of the PSoC board. Also, to view the data Tera Term is being used through UART protocol.

# PSoC-Schematic

<img width="499" height="312" alt="image" src="https://github.com/user-attachments/assets/8f5b1788-54ac-4e4e-a7ef-bdaae9fd952a" />

<img width="576" height="443" alt="image" src="https://github.com/user-attachments/assets/17f10342-640c-43e1-a90d-7f46b8f143c9" />

# PSoC-Code

```c
#include "project.h" // Project header file
#include <stdio.h> // Header file to use standard input and output functions
#include <string.h> // Header file to use string functions

// Sensor structure
typedef struct {
    char name[20];
    float value;
    char unit[10];
} Sensor;

// Simple 16-bit LFSR-based pseudo-random number generator
uint16_t lfsr = 0xACE1u;
uint16_t bit;

uint16_t getPseudoRandom() {
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr;
}

float getDummyValue(const char *name) {
    uint16_t r = getPseudoRandom();
    if (strcmp(name, "Temperature") == 0) return 25.0 + (r % 100) / 10.0;
    if (strcmp(name, "Pressure") == 0) return 101.3 + (r % 50) / 10.0;
    if (strcmp(name, "Humidity") == 0) return 40.0 + (r % 200) / 10.0;
    return 0;
}

void sendFormattedLineInt(const char* name, float value, const char* unit) {
    char buffer[64];
    int scaled = (int)(value * 10);  // Convert to integer (1 decimal place)
    sprintf(buffer, "%s: %d.%d %s\r\n", name, scaled / 10, scaled % 10, unit);
    UART_PutString(buffer);
    CyDelay(10);
}

int main(void) {
    CyGlobalIntEnable;
    UART_Start();
    CyDelay(100);  // Allow terminal time to open

    Sensor sensors[3] = {
        {"Temperature", 0, "C"},
        {"Pressure", 0, "kPa"},
        {"Humidity", 0, "%"}
    };

    UART_PutString("=== Virtual Sensor Dashboard ===\r\n\r\n");

    for (;;) {
        for (int i = 0; i < 3; i++) {
            sensors[i].value = getDummyValue(sensors[i].name);
            sendFormattedLineInt(sensors[i].name, sensors[i].value, sensors[i].unit);
        }
        UART_PutString("---------------------------\r\n\r\n");
        CyDelay(1000);
    }
}
```

# Purpose

In general in C programming, structures are used as a means to store the user or any specific data in an organized and manageable manner. Similarly, for the above code, the sensor data such as temperature, pressure and humidity can be stored in multiple array configurations as shown below but using structures offer flexibility to add or remove any number of options as per the requirement.

```c
char names[3][20] = {"Temperature", "Pressure", "Humidity"};
float values[3];
char units[3][10] = {"C", "kPa", "%"};
```

To conclude with an analogy, if the regular int, char or float data types are considered as a plain notebook of different sizes then structures can be considered as customized set of notebooks in which number of pages, columns, line width, design, etc. is printed as per the user requirements.

# Results (Tera Term)

<img width="436" height="455" alt="image" src="https://github.com/user-attachments/assets/1660ba7b-ebff-4f3c-a533-e145f6c4c068" />

**— END**
