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
#include "adxl345_int.hpp"
#include "adxl345_interface.hpp"

namespace peripheral
{
    /**
     * @brief ADXL345 模块的中间件。提供封装好的功能。
     */
    class adxl345_middleware : public adxl345_interface
    {
    public:
        adxl345_middleware()
        {
            software_reset();
        }
        ~adxl345_middleware()
        {
            reset_int1();
            reset_int2();
        }

    private:
        static constexpr int bit_shift(int x, int shift)
        {
            return x << shift;
        }
        static constexpr int bit_mask(int n)
        {
            return bit_shift(1, n);
        }
        template <typename... R>
        static constexpr int bit_mask(int n, R... others)
        {
            return bit_shift(1, n) | bit_mask(others...);
        }

    public:
        /**
         * @brief 三轴加速度信息。
         */
        struct accel_data_t
        {
            int16_t x;
            int16_t y;
            int16_t z;
        };

    public:
        /**
         * @brief 读器件 ID。器件 ID 为 0xE5 说明模块状态和时序基本正常。
         *
         * @see check_devid
         *
         * @return char 器件 ID。
         */
        char get_devid()
        {
            return read(adxl345_address::DEVID);
        }
        /**
         * @brief 检查器件 ID 是否为 0xE5。
         * 返回 true 说明模块状态和时序基本正常。
         */
        bool check_devid()
        {
            return get_devid() == 0xE5;
        }

        /**
         * @brief 软件重置 ADXL345。
         *
         * @note 软件重置 ADXL345 是未在文档中公开的方法，
         * 需要用到保留的地址 RESERVED1。
         */
        void software_reset()
        {
            // 向 RESERVED1 寄存器写 0x52 可以软件重置。
            write(adxl345_address::RESERVED1, 0x52);
        }

        /**
         * @brief 读加速度寄存器。
         */
        accel_data_t get_accel()
        {
            auto bytes = read<6>(adxl345_address::DATAX0);
            accel_data_t ret;
            ret.x = *reinterpret_cast<const int16_t*>(&bytes[0]);
            ret.y = *reinterpret_cast<const int16_t*>(&bytes[2]);
            ret.z = *reinterpret_cast<const int16_t*>(&bytes[4]);
            return ret;
        }

        /**
         * @brief 设置电源控制。
         *
         * @note 默认参数取自最小初始化序列。
         */
        void set_power_control(int wakeup = 0, bool sleep = false,
                               bool measure = true, bool auto_sleep = false,
                               bool link = false)
        {
            char data = bit_shift(wakeup & bit_mask(0, 1), 0) |
                        bit_shift(sleep, 2) | bit_shift(measure, 3) |
                        bit_shift(auto_sleep, 4) | bit_shift(link, 5);
            write(adxl345_address::POWER_CTL, data);
        }
        /**
         * @brief 获取电源控制。
         */
        char get_power_control()
        {
            return read(adxl345_address::POWER_CTL);
        }

        /**
         * @brief 设置数据格式。
         *
         * @note 默认参数取自最小初始化序列。
         */
        void set_data_format(int range = 3, bool justify = false,
                             bool full_res = true, bool int_invert = false,
                             bool spi_3 = false, bool self_test = false)
        {
            char data = bit_shift(range & bit_mask(0, 1), 0) |
                        bit_shift(justify, 2) | bit_shift(full_res, 3) |
                        bit_shift(int_invert, 5) | bit_shift(spi_3, 6) |
                        bit_shift(self_test, 7);
            write(adxl345_address::DATA_FORMAT, data);
        }
        /**
         * @brief 获取数据格式。
         */
        char get_data_format()
        {
            return read(adxl345_address::DATA_FORMAT);
        }

        /**
         * @brief 设置中断触发。
         *
         * @note 默认参数取自最小初始化序列。
         */
        void set_int_enable(
            adxl345_int::adxl345_int which = adxl345_int::DATAREADY)
        {
            write(adxl345_address::INT_ENABLE, static_cast<char>(which));
        }
        /**
         * @brief 获取中断触发。
         */
        adxl345_int::adxl345_int get_int_enable()
        {
            return static_cast<adxl345_int::adxl345_int>(
                read(adxl345_address::INT_ENABLE));
        }

        /**
         * @brief 设置中断映射。置 1 的位表示通过中断 2 管脚发送中断请求。
         */
        void set_int_map(adxl345_int::adxl345_int which)
        {
            write(adxl345_address::INT_MAP, static_cast<char>(which));
        }
        /**
         * @brief 获取中断映射。置 1 的位表示通过中断 2 管脚发送中断请求。
         */
        adxl345_int::adxl345_int get_int_map()
        {
            return static_cast<adxl345_int::adxl345_int>(
                read(adxl345_address::INT_MAP));
        }

        /**
         * @brief 读取中断源。
         * 除了 data_read (d7), watermark (d1), overrun (d0)，
         * 其他中断源将会在调用该函数时被清除。
         */
        adxl345_int::adxl345_int get_int_source()
        {
            return static_cast<adxl345_int::adxl345_int>(
                read(adxl345_address::INT_SOURCE));
        }

        /**
         * @brief 设置活动检测阈值。
         *
         * @note 复位后的初始值为 0。值为 0 时，该功能无法正常工作。
         *
         * @param threshold 阈值。比例因子为 62.5 mg/LSB。
         */
        void set_threshold_act(uint8_t threshold)
        {
            write(adxl345_address::THRESH_ACT, static_cast<char>(threshold));
        }
        /**
         * @brief 读取活动检测阈值。
         *
         * @return uint8_t 阈值。比例因子为 62.5 mg/LSB。
         */
        uint8_t get_threshold_act()
        {
            return static_cast<uint8_t>(read(adxl345_address::THRESH_ACT));
        }

        /**
         * @brief 设置活动/非活动检测功能的控制选项。
         *
         * @note 请参阅文档。
         */
        void set_act_inact_control(char control)
        {
            write(adxl345_address::ACT_INACT_CTL, control);
        }
        /**
         * @brief 读取活动/非活动检测功能的控制选项。
         */
        char get_act_inact_control()
        {
            return read(adxl345_address::ACT_INACT_CTL);
        }
    };
} // namespace peripheral
