/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>

extern "C" {

#include "platform.h"
#include "target.h"
#include "drivers/barometer/barometer.h"
#include "drivers/bus.h"

int8_t ms56xx_crc(uint16_t *prom);
void ms5611_calculate(int32_t *pressure, int32_t *temperature);

extern uint16_t ms56xx_c[8];
extern uint32_t ms56xx_up;
extern uint32_t ms56xx_ut;

}


#include "unittest_macros.h"
#include "gtest/gtest.h"


TEST(baroms5611Test, TestValidms5611Crc)
{
    // given
    uint16_t ms56xx_prom[] = {0x3132,0x3334,0x3536,0x3738,0x3940,0x4142,0x4344,0x450B};

    // when
    int8_t result = ms56xx_crc(ms56xx_prom);

    // then
    EXPECT_EQ(0, result);
}

TEST(baroms5611Test, TestInvalidms5611Crc)
{
    // given
    uint16_t ms56xx_prom[] = {0x3132,0x3334,0x3536,0x3738,0x3940,0x4142,0x4344,0x4500};

    // when
    int8_t result = ms56xx_crc(ms56xx_prom);

    // then
    EXPECT_EQ(-1, result);
}

TEST(baroms5611Test, Testms5611AllZeroProm)
{
    // given
    uint16_t ms56xx_prom[] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

    // when
    int8_t result = ms56xx_crc(ms56xx_prom);

    // then
    EXPECT_EQ(-1, result);
}

TEST(baroms5611Test, Testms5611AllOnesProm)
{
    // given
    uint16_t ms56xx_prom[] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};

    // when
    int8_t result = ms56xx_crc(ms56xx_prom);

    // then
    EXPECT_EQ(-1, result);
}

TEST(baroms5611Test, Testms5611CalculatePressureGT20Deg)
{
    // given
    int32_t pressure, temperature;
    uint16_t ms56xx_c_test[] = {0x0000, 40127, 36924, 23317, 23282, 33464, 28312, 0x0000}; // calibration data from ms56xx datasheet
    memcpy(&ms56xx_c, &ms56xx_c_test, sizeof(ms56xx_c_test));

    ms56xx_up = 9085466; // Digital pressure value from ms56xx datasheet
    ms56xx_ut = 8569150; // Digital temperature value from ms56xx datasheet

    // when
    ms5611_calculate(&pressure, &temperature);

    // then
    EXPECT_EQ(2007, temperature); // 20.07 deg C
    EXPECT_EQ(100009, pressure);  // 1000.09 mbar
}

TEST(baroms5611Test, Testms5611CalculatePressureLT20Deg)
{
    // given
    int32_t pressure, temperature;
    uint16_t ms56xx_c_test[] = {0x0000, 40127, 36924, 23317, 23282, 33464, 28312, 0x0000}; // calibration data from ms56xx datasheet
    memcpy(&ms56xx_c, &ms56xx_c_test, sizeof(ms56xx_c_test));

    ms56xx_up = 9085466; // Digital pressure value from ms56xx datasheet
    ms56xx_ut = 8069150; // Digital temperature value

    // when
    ms5611_calculate(&pressure, &temperature);

    // then
    EXPECT_EQ(205, temperature); // 2.05 deg C
    EXPECT_EQ(96512, pressure);  // 965.12 mbar
}

TEST(baroms5611Test, Testms5611CalculatePressureLTMinus15Deg)
{
    // given
    int32_t pressure, temperature;
    uint16_t ms56xx_c_test[] = {0x0000, 40127, 36924, 23317, 23282, 33464, 28312, 0x0000}; // calibration data from ms56xx datasheet
    memcpy(&ms56xx_c, &ms56xx_c_test, sizeof(ms56xx_c_test));

    ms56xx_up = 9085466; // Digital pressure value from ms56xx datasheet
    ms56xx_ut = 7369150; // Digital temperature value

    // when
    ms5611_calculate(&pressure, &temperature);

    // then
    EXPECT_EQ(-2710, temperature); // -27.10 deg C
    EXPECT_EQ(90613, pressure);  // 906.13 mbar
}

// STUBS

extern "C" {

void delay(uint32_t) {}
void delayMicroseconds(uint32_t) {}

bool busReadRegisterBuffer(const busDevice_t*, uint8_t, uint8_t*, uint8_t) {return true;}
bool busWriteRegister(const busDevice_t*, uint8_t, uint8_t) {return true;}

void spiSetDivisor() {
}

void spiPreinitCsByIO() {
}

void IOConfigGPIO() {
}

void IOHi() {
}

void IOInit() {
}

void IORelease() {
}

}
