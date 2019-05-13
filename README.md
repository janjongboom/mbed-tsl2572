# TSL2572 Mbed OS driver

Mbed OS 5 driver for the [TSL2572 Ambient Light Sensor](https://ams.com/tsl25721).

## Usage

```cpp
#include "TSL2572Sensor.h"

static TSL2572Sensor tsl2572(I2C_SDA, I2C_SCL);

int main() {
    // make sure to check the return values (should be 0)
    tsl2572.init();
    tsl2572.enable();

    uint8_t id;
    tsl2572.read_id(&id);
    printf("TSL2572 light intensity ID = 0x%X\r\n", id);

    float lux;
    tsl2572.read_ambient_light(&lux);
    printf("TSL2572: [lght] %.2f lux\r\n", lux);
}
```
