#include <project.h>
#include <stdio.h>

typedef union {
    float f;
    uint8_t bytes[4];
} FloatUnion;

int main(void) {
    CyGlobalIntEnable;
    UART_Start();

    FloatUnion sensorData;
    sensorData.f = 23.75f;

    char buffer[32];

    for (;;) {
        int intPart = (int)sensorData.f;
        int fracPart = (int)((sensorData.f - intPart) * 100);  // 2 decimal digits

        sprintf(buffer, "Float value: %d.%02d\r\n", intPart, fracPart);
        UART_PutString(buffer);

        CyDelay(1000);
    }
}