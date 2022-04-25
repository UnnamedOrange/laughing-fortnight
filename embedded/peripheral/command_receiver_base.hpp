/**
 * @file command_receiver_base.hpp
 * @author UnnamedOrange
 * @brief 接收指令的基类。该基类的子类可以接收指令。这个类被子模块使用。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>
#include <string>
#include <string_view>

namespace peripheral
{
    /**
     * @brief 接收指令的基类。该基类的子类可以接收指令。这个类被子模块使用。
     */
    class command_receiver_base
    {
    protected:
        /**
         * @brief 待实现的虚函数。阻塞地获取命令。
         *
         * @return std::string 获取的命令。
         */
        virtual std::string receive_command_impl_blocking() = 0;
        /**
         * @brief 待实现的虚函数。立即非阻塞地获取命令。
         *
         * @return std::string 获取的命令。
         */
        virtual std::string receive_command_impl_nonblocking() = 0;

    public:
        /**
         * @brief 阻塞地接收命令。在命令到来之前线程至少处于间歇性睡眠状态。
         *
         * @note 不保证用该方法接收到的命令是完整的。
         *
         * @return std::string 获取的命令。可能包含多行。
         */
        std::string receive_command()
        {
            return receive_command_impl_blocking();
        }
        /**
         * @brief 非阻塞地接收命令。线程睡眠 wait_time
         * 后，取得所有的命令然后立即返回。
         *
         * @param wait_time 获取命令前等待的时间。
         * @return std::string 获取的命令。可能包含多行，也可能为空。
         */
        std::string receive_command(Kernel::Clock::duration_u32 wait_time)
        {
            rtos::ThisThread::sleep_for(wait_time);
            return receive_command_impl_nonblocking();
        }
    };
} // namespace peripheral
