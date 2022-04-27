/**
 * @file bc26.hpp
 * @author UnnamedOrange
 * @brief BC26 模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>

#include "../command_receiver_serial.hpp"
#include "../command_sender_serial.hpp"
#include "../global_peripheral.hpp"
#include "../peripheral_std_framework.hpp"
#include "bc26_message.hpp"

namespace peripheral
{
    class bc26 : public peripheral_std_framework
    {
    protected:
        command_sender_serial sender{serial_bc26};
        command_receiver_serial receiver{serial_bc26};

        /**
         * @brief 以下函数是子模块的回调函数，均在子线程中运行。
         */
    private:
        void on_message(int id, std::shared_ptr<void> data) override
        {
            switch (static_cast<bc26_message_t>(id))
            {
            case bc26_message_t::send_at:
            {
                on_send_at(*std::static_pointer_cast<int>(data));
                break;
            }
            default:
            {
                break;
            }
            }
        }
        /**
         * @brief 重复发送 AT 指令，直到收到 OK。
         *
         * @param max_retry 最大重试次数。
         */
        void on_send_at(int max_retry) // 参见 bc26_message_t::send_at。
        {
            bool is_success = false;
            std::string received_str;
            for (int i = 0; i < max_retry; i++)
            {
                using namespace std::literals;
                utils::debug_printf("[-] AT\n");
                sender.send_command("AT\r\n");
                received_str = receiver.receive_command(300ms);
                utils::debug_printf("%s", received_str.c_str());
                if (received_str.find("OK") != std::string::npos)
                {
                    utils::debug_printf("[D] AT\n");
                    is_success = true;
                    break;
                }
                else
                    utils::debug_printf("[F] AT\n");
            }

            // TODO: 根据结果向主模块反馈。
        }

        /**
         * @brief 以下函数是主模块的接口，均在主线程中运行。
         */
    public:
        /**
         * @brief 向子模块发送消息。重复发送 AT 指令，直到收到 OK。
         *
         * @param max_retry 最大重试次数。
         */
        void send_at(int max_retry = 10)
        {
            push(static_cast<int>(bc26_message_t::send_at),
                 std::make_shared<int>(max_retry));
        }
    };
} // namespace peripheral
