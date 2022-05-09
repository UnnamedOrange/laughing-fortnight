/**
 * @file adxl345_interface.hpp
 * @author UnnamedOrange
 * @brief ADXL345 专用底层模块。根据 ADXL345 模块时序设计的读写接口。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <array>
#include <string>
#include <utility>

#include "../command_spi.hpp"
#include "../global_peripheral.hpp"

namespace peripheral
{
    /**
     * @brief 根据 ADXL345 模块时序设计的读写接口。
     *
     * @note 继承此类实现中间件。
     */
    class adxl345_interface
    {
    private:
        mbed::SPI spi_accel{PIN_ACCEL_MOSI, PIN_ACCEL_MISO, PIN_ACCEL_SCLK};
        mbed::DigitalOut cs_accel{PIN_ACCEL_CS};
        peripheral::command_spi<> spi{spi_accel, cs_accel};
        mbed::InterruptIn accel_int1{PIN_ACCEL_INT1};
        mbed::InterruptIn accel_int2{PIN_ACCEL_INT2};

    protected:
        /**
         * @brief 写 ADXL345 寄存器。
         *
         * @param addr 写入的地址。
         * @param native_array 以数组形式组织的待写入数据。
         */
        template <size_t size>
        void write(char addr, const char (&native_array)[size])
        {
            std::array<char, size + 1> data{addr};
            std::copy(native_array, native_array + size, data.begin() + 1);
            spi.write(data);
        }
        /**
         * @brief 连续读 ADXL345 寄存器。
         *
         * @tparam size 读出数据的数量。
         * @param addr 读出的地址。
         * @return std::string 读出的数据。长度等于 size。
         */
        template <size_t size>
        std::string read(char addr)
        {
            static_assert(size >= 2, "size must not be less than 2.");
            // 最高位表示读，次高位表示连续读取。
            std::array<char, size + 1> to_write{
                static_cast<char>(addr | 0x80 | 0x40)};
            auto received = spi.write(to_write);
            // 注意 std::string 构造函数的含义。
            return std::string(received.begin() + 1, received.end());
        }
        /**
         * @brief 读 ADXL345 寄存器。
         *
         * @param addr 读出的地址。
         * @return char 读出的数据。是单个字节。
         */
        char read(char addr)
        {
            // 最高位表示读，次高位表示连续读取。
            std::array<char, 2> to_write{
                static_cast<char>(addr | 0x80 & (~0x40))};
            auto received = spi.write(to_write);
            return received[1];
        }

    public:
        /**
         * @brief 设置 INT1 的中断处理程序。
         */
        void set_int1(mbed::Callback<void()> irq_callback)
        {
            accel_int1.rise(irq_callback);
        }
        /**
         * @brief 清除 INT1 的中断处理程序。
         */
        void reset_int1()
        {
            accel_int1.rise(nullptr);
        }
        /**
         * @brief 设置 INT2 的中断处理程序。
         */
        void set_int2(mbed::Callback<void()> irq_callback)
        {
            accel_int2.rise(irq_callback);
        }
        /**
         * @brief 清除 INT2 的中断处理程序。
         */
        void reset_int2()
        {
            accel_int2.rise(nullptr);
        }
    };
} // namespace peripheral
