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
#include "../feedback_message.hpp"
#include "../feedback_message_queue.hpp"
#include "../global_peripheral.hpp"
#include "../peripheral_std_framework.hpp"
#include "bc26_message.hpp"

namespace peripheral
{
    class bc26 : public peripheral_std_framework
    {
    private:
        using _fmq_t = feedback_message_queue;
        using _fmq_e_t = feedback_message_enum_t;

    protected:
        command_sender_serial sender{serial_bc26};
        command_receiver_serial receiver{serial_bc26};
        _fmq_t& _external_fmq;

    public:
        bc26(_fmq_t& fmq) : _external_fmq(fmq)
        {
        }

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
            case bc26_message_t::send_ate:
            {
                on_send_ate(*std::static_pointer_cast<bool>(data));
                break;
            }
            case bc26_message_t::send_at_cfun_set:
            {
                on_send_at_cfun_set(*std::static_pointer_cast<int>(data));
                break;
            }
            case bc26_message_t::send_at_cimi:
            {
                on_send_at_cimi();
                break;
            }
            case bc26_message_t::send_at_cgatt_get:
            {
                on_send_at_cgatt_get();
                break;
            }
            case bc26_message_t::send_at_cesq:
            {
                on_send_at_cesq();
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
        void on_send_at(int max_retry,
                        _fmq_t& fmq) // 参见 bc26_message_t::send_at。
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

            // 参见 feedback_message_enum_t::bc26_send_at。
            fmq.post_message(_fmq_e_t::bc26_send_at,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at(int max_retry)
        {
            on_send_at(max_retry, _external_fmq);
        }
        /**
         * @brief 发送 ATE 指令，打开或关闭回显。
         *
         * @param is_echo 是否打开回显。
         */
        void on_send_ate(bool is_echo,
                         _fmq_t& fmq) // 参见 bc26_message_t::send_ate。
        {
            utils::debug_printf("[-] ATE%d\n", static_cast<int>(is_echo));
            sender.send_command("ATE" + std::to_string(is_echo) + "\r\n");
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());
            utils::debug_printf("[D] ATE%d\n", static_cast<int>(is_echo));

            bool is_success = received_str.find("OK") != std::string::npos;
            // 参见 feedback_message_enum_t::bc26_send_ate。
            fmq.post_message(_fmq_e_t::bc26_send_ate,
                             std::make_shared<bool>(is_success));
        }
        void on_send_ate(bool is_echo)
        {
            on_send_ate(is_echo, _external_fmq);
        }
        /**
         * @brief 发送 AT+CFUN=<mode> 指令。设置功能模式。
         *
         * @param mode 功能模式。
         */
        void on_send_at_cfun_set(
            int mode,
            _fmq_t& fmq) // 参见 bc26_message_t::send_at_cfun_set。
        {
            utils::debug_printf("[-] AT+CFUN=%d\n", mode);
            sender.send_command("AT+CFUN=" + std::to_string(mode) + "\r\n");
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());
            utils::debug_printf("[D] AT+CFUN=%d\n", mode);

            bool is_success = received_str.find("OK") != std::string::npos;
            // 参见 feedback_message_enum_t::bc26_send_at_cfun_set。
            fmq.post_message(_fmq_e_t::bc26_send_at_cfun_set,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at_cfun_set(int mode)
        {
            on_send_at_cfun_set(mode, _external_fmq);
        }
        /**
         * @brief 发送 AT+CIMI 指令。查询卡号。
         */
        void on_send_at_cimi(_fmq_t& fmq) // 参见 bc26_message_t::send_at_cimi。
        {
            utils::debug_printf("[-] AT+CIMI\n");
            sender.send_command("AT+CIMI\r\n");
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());
            utils::debug_printf("[D] AT+CIMI\n");

            // TODO: 解析更多信息。
            bool is_success = received_str.find("OK") != std::string::npos;
            // 参见 feedback_message_enum_t::bc26_send_at_cimi。
            fmq.post_message(_fmq_e_t::bc26_send_at_cimi,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at_cimi()
        {
            on_send_at_cimi(_external_fmq);
        }
        /**
         * @brief 发送 AT_CGATT? 指令。查询激活状态。
         */
        void on_send_at_cgatt_get(
            _fmq_t& fmq) // 参见 bc26_message_t::send_at_cgatt_get。
        {
            utils::debug_printf("[-] AT+CGATT?\n");
            sender.send_command("AT+CGATT?\r\n");
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());
            utils::debug_printf("[D] AT+CGATT?\n");

            // TODO: 解析更多信息。
            bool is_success = received_str.find("OK") != std::string::npos;
            // 参见 feedback_message_enum_t::bc26_send_at_cgatt_get。
            fmq.post_message(_fmq_e_t::bc26_send_at_cgatt_get,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at_cgatt_get()
        {
            on_send_at_cgatt_get(_external_fmq);
        }
        /**
         * @brief 发送 AT+CESQ 指令。获取信号质量。
         */
        void on_send_at_cesq(_fmq_t& fmq) // 参见 bc26_message_t::send_at_cesq。
        {
            utils::debug_printf("[-] AT+CESQ\n");
            sender.send_command("AT+CESQ\r\n");
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());
            utils::debug_printf("[D] AT+CESQ\n");

            // TODO: 解析更多信息。
            bool is_success = received_str.find("OK") != std::string::npos;
            // 参见 feedback_message_enum_t::bc26_send_at_cesq。
            fmq.post_message(_fmq_e_t::bc26_send_at_cesq,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at_cesq()
        {
            on_send_at_cesq(_external_fmq);
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
        /**
         * @brief 向子模块发送消息。发送 ATE<echo> 指令。打开或关闭回显。
         *
         * @param is_echo 是否打开回显。默认为不打开。
         */
        void send_ate(bool is_echo = false)
        {
            push(static_cast<int>(bc26_message_t::send_ate),
                 std::make_shared<bool>(is_echo));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+CFUN=<mode> 指令。设置功能模式。
         *
         * @param mode 功能模式。默认为 1。
         */
        void send_at_cfun_set(int mode = 1)
        {
            push(static_cast<int>(bc26_message_t::send_at_cfun_set),
                 std::make_shared<int>(mode));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+CIMI 指令。查询卡号。
         */
        void send_at_cimi()
        {
            push(static_cast<int>(bc26_message_t::send_at_cimi), nullptr);
        }
        /**
         * @brief 向子模块发送消息。发送 AT_CGATT? 指令。查询激活状态。
         */
        void send_at_cgatt_get()
        {
            push(static_cast<int>(bc26_message_t::send_at_cgatt_get), nullptr);
        }
        /**
         * @brief 向子模块发送消息。发送 AT+CESQ 指令。获取信号质量。
         */
        void send_at_cesq()
        {
            push(static_cast<int>(bc26_message_t::send_at_cesq), nullptr);
        }
    };
} // namespace peripheral
