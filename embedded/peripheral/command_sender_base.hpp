/**
 * @file command_sender_base.hpp
 * @author UnnamedOrange
 * @brief 发送指令的基类。该基类的子类可以发送指令。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <string_view>

namespace peripheral
{
    /**
     * @brief 发送指令的基类。该基类的子类可以发送指令。
     */
    class command_sender_base
    {
    public:
        /**
         * @brief 待实现的虚函数。用不同的实现方式发送指令。
         *
         * @param command 指令。没有自动换行。
         */
        virtual void send_command(std::string_view command) = 0;
    };
} // namespace peripheral
