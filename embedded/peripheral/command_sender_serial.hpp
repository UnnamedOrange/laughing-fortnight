/**
 * @file command_sender_serial.hpp
 * @author UnnamedOrange
 * @brief 使用串口发送指令。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <string_view>

#include "command_sender_base.hpp"

namespace peripheral
{
    /**
     * @brief 通过串口发送指令。
     *
     * @note 注意不使用串口的 RX。只发不收。
     * 接收指令的类另写。
     *
     * @note 这个类是线程安全的，@ref send_command 类可重入。
     * 但同一模板类的不同对象不是线程安全的。应避免同一模板类具有多个对象。
     *
     * @todo 测试这个类。
     *
     * @tparam PIN_TX 串口的 TX 管脚（CPU 端）。
     * @tparam baud 串口的波特率。默认为配置文件中设置的值，默认 9600。
     */
    template <PinName PIN_TX,
              int baud = MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE>
    class command_sender_serial : public command_sender_base
    {
    private:
        mbed::BufferedSerial _serial{PIN_TX, NC, baud};

    public:
        /**
         * @brief 通过串口发送指令。
         *
         * @note 该函数可重入。但同一模板类的不同对象不是线程安全的。
         *
         * @note 默认是阻塞的。
         *
         * @param command 指令。没有自动换行。
         */
        void send_command(std::string_view command) override
        {
            // 函数内部会针对 _serial 对象加锁。
            _serial.write(command.data(), command.length());
        }
    };
} // namespace peripheral
