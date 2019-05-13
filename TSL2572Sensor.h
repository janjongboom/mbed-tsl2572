/**
 * Copyright (c) 2019, Jan Jongboom
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"

#ifndef TSL2572_SENSOR_H
#define TSL2572_SENSOR_H

#define TSL2572_I2CADDR     0x39 << 1

#define TSL25721            0x34
#define TSL25723            0x3D

#define TSL2572_GAIN_1X     1
#define TSL2572_GAIN_8X     8
#define TSL2572_GAIN_16X    16
#define TSL2572_GAIN_120X   120

#define TSL2572_CMD_REGISTER        0x80

#define TSL2572_CMDS_ENABLE         0x00
#define TSL2572_CMDS_ALS_TIMING     0x01
#define TSL2572_CMDS_WAIT_TIME      0x03
#define TSL2572_CMDS_PERSISTANCE    0x0c
#define TSL2572_CMDS_CONFIG         0x0d
#define TSL2572_CMDS_CONTROL        0x0f
#define TSL2572_CMDS_WHOAMI         0x12
#define TSL2572_CMDS_STATUS         0x13

class TSL2572Sensor {
public:
    /**
     * Instantiate the TSL2572 driver
     * @param sda I2C SDA pin
     * @param scl I2C SCL pin
     * @param address I2C address, default TSL2572_I2CADDR
     */
    TSL2572Sensor(PinName sda, PinName scl, uint8_t address = TSL2572_I2CADDR);

    /**
     * Initialize the driver. This executes the ID command, and verifies the output.
     * @returns 0 if OK, a positive number if I2C read/write failed, -1 if cannot detect the chip
     */
    int init();

    /**
     * Enable the sensor.
     * @param gain ALS gain control to the analog block. Default TSL2572_GAIN_1X.
     * @returns 0 if OK, a positive number if I2C read/write failed.
     */
    int enable(uint8_t gain = TSL2572_GAIN_1X);

    /**
     * Disable the sensor.
     * @returns 0 if OK, a positive number if I2C read/write failed.
     */
    int disable();

    /**
     * Read the current ambient light value
     * @param value Pointer to value
     * @returns 0 if OK, a positive number if I2C read/write failed, -1 if sensor is not enabled.
     */
    int read_ambient_light(float *value);

    /**
     * Read Who am I register of the chip
     * @param id Pointer to ID
     * @return 0 if OK, a positive value if I2C read failed
     */
    int read_id(uint8_t *id);

private:
    I2C _i2c;
    uint8_t _address;

    uint8_t _identifier;
    bool _enabled;
    uint8_t _gain;
};

#endif /* TSL2572_SENSOR_H */
