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

#include "TSL2572Sensor.h"

TSL2572Sensor::TSL2572Sensor(PinName sda, PinName scl, uint8_t address)
        : _i2c(sda, scl), _address(address), _enabled(false)
{}

int TSL2572Sensor::init() {
    // whoami command (0x12)
    char command[] = { TSL2572_CMD_REGISTER | TSL2572_CMDS_WHOAMI };
    int r = _i2c.write(_address, command, 1);
    if (r != 0) {
        return r;
    }

    r = _i2c.read(_address, command, 1);
    if (r != 0) {
        return r;
    }

    if (command[0] != TSL25721 && command[0] != TSL25723) {
        return -1;
    }

    _identifier = command[0];

    return 0;
}

int TSL2572Sensor::enable(uint8_t gain) {
    _gain = gain;

    // Set gain
    {
        char command[2] = { TSL2572_CMD_REGISTER | TSL2572_CMDS_CONTROL, gain };
        int r = _i2c.write(_address, command, sizeof(command));
        if (r != 0) {
            return r;
        }
    }

    // Set timing register
    {
        char command[2] = { TSL2572_CMD_REGISTER | TSL2572_CMDS_ALS_TIMING, 0xed }; // 51.87 ms
        int r = _i2c.write(_address, command, sizeof(command));
        if (r != 0) {
            return r;
        }
    }

    // Enable AEN and PON
    {
        char command[2] = { TSL2572_CMD_REGISTER | TSL2572_CMDS_ENABLE, 0b00000011 };
        int r = _i2c.write(_address, command, sizeof(command));
        if (r != 0) {
            return r;
        }
    }

    // Scale gain (only on 1x and 8x)
    if (gain == TSL2572_GAIN_1X || gain == TSL2572_GAIN_8X) {
        char command[2] = { TSL2572_CMD_REGISTER | TSL2572_CMDS_CONFIG, 0x04 }; //scale gain by 0.16
        int r = _i2c.write(_address, command, sizeof(command));
        if (r != 0) {
            return r;
        }
    }

    _enabled = true;

    return 0;
}

int TSL2572Sensor::disable() {
    // Disable everything
    {
        char command[2] = { TSL2572_CMD_REGISTER | TSL2572_CMDS_ENABLE, 0b00000000 };
        int r = _i2c.write(_address, command, sizeof(command));
        if (r != 0) {
            return r;
        }
    }

    _enabled = false;

    return 0;
}

int TSL2572Sensor::read_ambient_light(float *value) {
    if (!_enabled) {
        *value = 0.0f;
        return -1;
    }

    char command[4] = { 0xa0 | 0x14, 0x0, 0x0, 0x0 };
    int r = _i2c.write(_address, command, 1);
    if (r != 0) {
        return r;
    }

    r = _i2c.read(_address, command, sizeof(command));
    if (r != 0) {
        return r;
    }

    int c0, c1;
    float lux1, lux2, cpl;

    c0 = command[1] << 8 | command[0];
    c1 = command[3] << 8 | command[2];

    cpl = 51.87f * static_cast<float>(_gain) / 60.0f;
    if (_gain == TSL2572_GAIN_1X || _gain == TSL2572_GAIN_8X) {
        // scale gain
        cpl /= 6.0f;
    }
    lux1 = (static_cast<float>(c0) - (1.87f * static_cast<float>(c1))) / cpl;
    lux2 = ((0.63f * static_cast<float>(c0)) - static_cast<float>(c1)) / cpl;
    cpl = max(lux1, lux2);

    *value = max(cpl, 0.0f);

    return 0;
}

int TSL2572Sensor::read_id(uint8_t *id) {
    // already cached
    *id = _identifier;
    return 0;
}
