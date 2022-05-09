/**
 * @file adxl345_address.hpp
 * @author UnnamedOrange
 * @brief 定义 ADXL345 的寄存器地址。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

namespace peripheral
{
    namespace adxl345_address
    {
        /**
         * @brief ADXL345 模块的寄存器地址。
         */
        enum adxl345_address : char
        {
            DEVID = 0x00,
            RESERVED1 = 0x01,
            THRESH_TAP = 0x1d,
            OFSX = 0x1e,
            OFSY = 0x1f,
            OFSZ = 0x20,
            DUR = 0x21,
            LATENT = 0x22,
            WINDOW = 0x23,
            THRESH_ACT = 0x24,
            THRESH_INACT = 0x25,
            TIME_INACT = 0x26,
            ACT_INACT_CTL = 0x27,
            THRESH_FF = 0x28,
            TIME_FF = 0x29,
            TAP_AXES = 0x2a,
            ACT_TAP_STATUS = 0x2b,
            BW_RATE = 0x2c,
            POWER_CTL = 0x2d,
            INT_ENABLE = 0x2e,
            INT_MAP = 0x2f,
            INT_SOURCE = 0x30,
            DATA_FORMAT = 0x31,
            DATAX0 = 0x32,
            DATAX1 = 0x33,
            DATAY0 = 0x34,
            DATAY1 = 0x35,
            DATAZ0 = 0x36,
            DATAZ1 = 0x37,
            FIFO_CTL = 0x38,
            FIFO_STATUS = 0x39,
        };
    } // namespace adxl345_address
} // namespace peripheral
