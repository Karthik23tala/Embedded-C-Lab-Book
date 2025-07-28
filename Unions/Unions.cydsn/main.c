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