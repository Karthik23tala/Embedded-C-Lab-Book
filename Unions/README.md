# Unions - Data Transfer

# Objective

To demonstrate how to convert a float value (for e.g: any sensor data) to bytes format for transmission and reconstruct it accurately on the receiving end using a union data type.

# Working

This lab is simply about reconstructing the value that is being sent from one device and being received to the other device which can be PC/Laptop to PSoC development board. For instance, in this case, a float value cycling from -30 to +30 is being sent to the PSoC board through union data type whereby the entire float data of 4 bytes is sent one byte each time. The same breakdown of the float value to bytes can be viewed in the bytes section in the output window of the Tera Term.

# PSoC-Schematic

<img width="499" height="312" alt="image" src="https://github.com/user-attachments/assets/8083363d-be52-421d-9c96-9b3504efbb5b" />

<img width="576" height="443" alt="image" src="https://github.com/user-attachments/assets/67b73668-7023-4754-be5f-a0c11a12710b" />

# PSoC-Code

```c
#include <project.h>
#include <stdio.h>
#include <math.h>

typedef union {
    float f;
    uint8_t bytes[4];
} FloatUnion;
char txBuffer[128];

void sendHexBytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        sprintf(txBuffer, "0x%02X ", data[i]);
        UART_PutString(txBuffer);
    }
    UART_PutString("\r\n");
}

void sendFloatFormatted(const char *label, float value) {
    int whole = (int)value;
    int frac = (int)((fabs(value - whole)) * 100);
    sprintf(txBuffer, "%s: %d.%02d\r\n", label, whole, frac);
    UART_PutString(txBuffer);
}

int main(void) {
    CyGlobalIntEnable;
    UART_Start();
    float angle = -30.5f;  // Starting value
    FloatUnion original;
    FloatUnion reconstructed;
    for (;;) {
        // Simulate changing sensor reading
        original.f = angle;
        // Break into bytes and send
        sendFloatFormatted("Original", original.f);
        UART_PutString("Bytes: ");
        sendHexBytes(original.bytes, 4);
        // Simulate sending and receiving the same bytes
        for (int i = 0; i < 4; i++) {
            reconstructed.bytes[i] = original.bytes[i];
        }
        // Send reconstructed float
        sendFloatFormatted("Reconstructed", reconstructed.f);
        UART_PutString("-------------------------\r\n");
        angle += 0.5f;  // Simulate reading change
        if (angle > 30.0f) angle = -30.0f;  // Cycle
        CyDelay(1000);
    }
}
```

# Purpose

Similar to the structures used in previous lab example, unions are also used to store the data in organized manner in C programming. But the primary difference between structures and unions is that in the later one all the blocks should be of same data type. This definition can be int, float or char but only one data length across the elements of the union. For instance, in the above example there are two variables one is float “f” of 4 bytes and the another is 4-element array “bytes” with each element of size one byte.

# Results (Tera Term)

<img width="495" height="644" alt="image" src="https://github.com/user-attachments/assets/6099ba45-a625-4a2d-b97f-db7032963319" />

**— END**
