/**
 * @file command_receiver_serial.hpp
 * @author UnnamedOrange
 * @brief 使用串口接收指令。这个类被子模块使用。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <array>
#include <chrono>
#include <string>
#include <string_view>

#include "command_receiver_base.hpp"

namespace peripheral
{
    /**
     * @brief 通过串口接收指令。
     *
     * @note 注意不使用串口的 TX。只收不发。
     *
     * @note 串口必须工作在双工模式下，所以需要传入一个全局对象的引用。
     *
     * @note 考虑到线程安全问题，同一个 BufferedSerial 应该只有一个 sender 和
     * receiver。
     *
     * @note 这个类本身不是线程安全的。
     */
    class command_receiver_serial : public command_receiver_base
    {
    private:
        mbed::BufferedSerial& _serial;

    public:
        command_receiver_serial(mbed::BufferedSerial& serial) : _serial(serial)
        {
        }

    private:
        // 共用的读取函数。注意需要同时考虑阻塞式和非阻塞式的情况。
        std::string _read()
        {
            // 缓冲区大小设为与串口收端相同的缓冲区大小。
            constexpr size_t buffer_size =
                MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE;
            // 注意 std::string 对 0 的处理。
            std::string buffer(buffer_size, '\0');
            // read 函数返回值的类型是有符号的 size_t。
            auto n_bytes_read =
                _serial.read(const_cast<char*>(buffer.data()), buffer.size());
            // 非阻塞式读取时，什么都没读到将返回一个负数。
            if (n_bytes_read == -EAGAIN) // < 0。
            {
                buffer.clear();
                return buffer;
            }
            // 注意 std::string 对 0 的处理。
            buffer.resize(static_cast<size_t>(n_bytes_read));
            return buffer;
        }

    private:
        /**
         * @brief 使用串口阻塞地获取命令。
         *
         * @return std::string 获取的命令。
         */
        std::string receive_command_impl_blocking() override
        {
            _serial.set_blocking(true);
            return _read();
        }
        /**
         * @brief 使用串口立即非阻塞地获取命令。
         *
         * @return std::string 获取的命令。
         */
        std::string receive_command_impl_nonblocking() override
        {
            _serial.set_blocking(false);
            return _read();
        }
    };
} // namespace peripheral
