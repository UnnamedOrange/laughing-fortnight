/**
 * @file adxl345_int.hpp
 * @author UnnamedOrange
 * @brief 定义 ADXL345 的中断标志位。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

namespace peripheral
{
    namespace adxl345_int
    {
        /**
         * @brief ADXL345 模块的中断标志位。
         */
        enum adxl345_int : char
        {
            OVERRUN = 0x01,
            WATERMARK = 0x02,
            FREEFALL = 0x04,
            INACTIVITY = 0x08,
            ACTIVITY = 0x10,
            DOUBLETAP = 0x20,
            SINGLETAP = 0x40,
            DATAREADY = 0x80,
        };
    } // namespace adxl345_int
} // namespace peripheral
