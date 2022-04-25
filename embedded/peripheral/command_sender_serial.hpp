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
     * @note 串口必须工作在双工模式下，所以需要传入一个全局对象的引用。
     *
     * @note 考虑到线程安全问题，同一个 BufferedSerial 应该只有一个 sender 和
     * receiver。
     *
     * @note 这个类本身是线程安全的。
     */
    class command_sender_serial : public command_sender_base
    {
    private:
        mbed::BufferedSerial& _serial;

    public:
        command_sender_serial(mbed::BufferedSerial& serial) : _serial(serial)
        {
        }

    public:
        /**
         * @brief 通过串口发送指令。
         *
         * @note 该函数可重入。
         *
         * @note 是否阻塞是不确定的。
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
