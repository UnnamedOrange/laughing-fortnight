/**
 * @file adxl345_middleware.hpp
 * @author UnnamedOrange
 * @brief ADXL345 模块的中间件。提供封装好的功能。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include "adxl345_address.hpp"
#include "adxl345_interface.hpp"

namespace peripheral
{
    /**
     * @brief ADXL345 模块的中间件。提供封装好的功能。
     */
    class adxl345_middleware : public adxl345_interface
    {
    public:
        /**
         * @brief 读器件 ID。器件 ID 为 0xE5 说明模块状态和时序基本正常。
         *
         * @see check_devid
         *
         * @return char 器件 ID。
         */
        char read_devid()
        {
            return read(adxl345_address::DEVID);
        }
        /**
         * @brief 检查器件 ID 是否为 0xE5。
         * 返回 true 说明模块状态和时序基本正常。
         */
        bool check_devid()
        {
            return read_devid() == 0xE5;
        }
    };
} // namespace peripheral
