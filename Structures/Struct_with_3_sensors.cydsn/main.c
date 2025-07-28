#include "project.h" //User defined header file
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