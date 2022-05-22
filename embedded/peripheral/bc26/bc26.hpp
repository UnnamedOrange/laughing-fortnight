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
#include <string>
#include <tuple>
#include <vector>

#include "../command_receiver_serial.hpp"
#include "../command_sender_serial.hpp"
#include "../feedback_message.hpp"
#include "../feedback_message_queue.hpp"
#include "../global_peripheral.hpp"
#include "../peripheral_std_framework.hpp"
#include "bc26_message.hpp"
#include <utils/debug.hpp>
#include <utils/msg_data.hpp>

namespace peripheral
{
    class bc26 : public peripheral_std_framework
    {
    private:
        using _fmq_t = feedback_message_queue;
        using _fmq_e_t = feedback_message_enum_t;

    protected:
        mbed::BufferedSerial serial_bc26{PIN_BC26_TX, PIN_BC26_RX};
        command_sender_serial sender{serial_bc26};
        command_receiver_serial receiver{serial_bc26};
        _fmq_t& _external_fmq;

    public:
        bc26(_fmq_t& fmq) : _external_fmq(fmq)
        {
        }
        ~bc26()
        {
            descendant_exit();
        }

        // 以下函数是子模块的回调函数，均在子线程中运行。
    private:
        void on_message(int id, std::shared_ptr<void> data) override
        {
            descendant_callback_begin();
            switch (static_cast<bc26_message_t>(id))
            {
            case bc26_message_t::send_at:
            {
                on_send_at(*std::static_pointer_cast<int>(data));
                break;
            }
            case bc26_message_t::software_reset:
            {
                on_software_reset();
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
            case bc26_message_t::init:
            {
                on_init(*std::static_pointer_cast<int>(data));
                break;
            }
            case bc26_message_t::send_at_qiopen:
            {
                using param_type = std::tuple<std::string, int, int, bool>;
                const auto& param = *std::static_pointer_cast<param_type>(data);
                on_send_at_qiopen(std::get<0>(param), std::get<1>(param),
                                  std::get<2>(param), std::get<3>(param));
                break;
            }
            case bc26_message_t::send_at_qiclose:
            {
                auto connect_id = *std::static_pointer_cast<int>(data);
                on_send_at_qiclose(connect_id);
                break;
            }
            case bc26_message_t::send_at_qisend:
            {
                using param_type = std::tuple<std::string, int>;
                const auto& param = *std::static_pointer_cast<param_type>(data);
                on_send_at_qisend(std::get<0>(param), std::get<1>(param));
                break;
            }
            case bc26_message_t::send_at_qird:
            {
                auto connect_id = *std::static_pointer_cast<int>(data);
                on_send_at_qird(connect_id);
                break;
            }
            case bc26_message_t::send_at_qmtcfg:
            {
                using param_type =
                    std::tuple<std::string, std::vector<std::string>>;
                const auto& param = *std::static_pointer_cast<param_type>(data);
                on_send_at_qmtcfg(std::get<0>(param), std::get<1>(param));
                break;
            }
            case bc26_message_t::send_at_qmtopen:
            {
                using param_type = std::tuple<int, std::string, int>;
                const auto& param = *std::static_pointer_cast<param_type>(data);
                on_send_at_qmtopen(std::get<0>(param), std::get<1>(param),
                                   std::get<2>(param));
                break;
            }
            case bc26_message_t::send_at_qmtclose:
            {
                auto tcp_connect_id = *std::static_pointer_cast<int>(data);
                on_send_at_qmtclose(tcp_connect_id);
                break;
            }
            case bc26_message_t::send_at_qmtconn:
            {
                using param_type =
                    std::tuple<int, std::string, std::string, std::string>;
                const auto& param = *std::static_pointer_cast<param_type>(data);
                on_send_at_qmtconn(std::get<0>(param), std::get<1>(param),
                                   std::get<2>(param), std::get<3>(param));
                break;
            }
            case bc26_message_t::send_at_qmtdisc:
            {
                auto tcp_connect_id = *std::static_pointer_cast<int>(data);
                on_send_at_qmtdisc(tcp_connect_id);
                break;
            }
            case bc26_message_t::send_at_qmtsub:
            {
                using param_type = std::tuple<int, int, std::string, int>;
                const auto& param = *std::static_pointer_cast<param_type>(data);
                on_send_at_qmtsub(std::get<0>(param), std::get<1>(param),
                                  std::get<2>(param), std::get<3>(param));
                break;
            }
            default:
            {
                break;
            }
            }
            descendant_callback_end();
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
         * @brief 发送 AT+QRST=1 指令。软件重置。
         */
        void on_software_reset(_fmq_t& fmq)
        {
            utils::debug_printf("[-] AT+QRST=1\n");
            sender.send_command("AT+QRST=1\r\n");
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());
            utils::debug_printf("[D] AT+QRST=1\n");

            // 参见 feedback_message_enum_t::bc26_software_reset。
            fmq.post_message(_fmq_e_t::bc26_software_reset, nullptr);
        }
        void on_software_reset()
        {
            on_software_reset(_external_fmq);
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

            bool is_success = received_str.find("OK") != std::string::npos;
            utils::debug_printf("[%c] ATE%d\n", is_success ? 'D' : 'F',
                                static_cast<int>(is_echo));
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

            bool is_success = received_str.find("OK") != std::string::npos;
            utils::debug_printf("[%c] AT+CFUN=%d\n", is_success ? 'D' : 'F',
                                mode);
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

            bool is_success = received_str.find("OK") != std::string::npos;
            char id[32]{};
            // 解析失败时，id 应该为全 0。
            if (is_success && 1 != sscanf(received_str.c_str(), "%s", id))
                is_success = false;
            utils::debug_printf("[%c] AT+CIMI\n", is_success ? 'D' : 'F');
            // 参见 feedback_message_enum_t::bc26_send_at_cimi。
            fmq.post_message(_fmq_e_t::bc26_send_at_cimi,
                             std::make_shared<std::tuple<bool, std::string>>(
                                 is_success, id));
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

            bool is_success = received_str.find("OK") != std::string::npos;
            int is_activated{};
            if (is_success && 1 != sscanf(received_str.c_str(),
                                          "\r\n+CGATT: %d", &is_activated))
                is_success = false;
            utils::debug_printf("[%c] AT+CGATT?\n", is_success ? 'D' : 'F');
            // 参见 feedback_message_enum_t::bc26_send_at_cgatt_get。
            fmq.post_message(_fmq_e_t::bc26_send_at_cgatt_get,
                             std::make_shared<std::tuple<bool, bool>>(
                                 is_success, is_activated));
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

            bool is_success = received_str.find("OK") != std::string::npos;
            int intensity{};
            if (is_success &&
                1 != sscanf(received_str.c_str(), "\r\n+CESQ: %d", &intensity))
                is_success = false;
            utils::debug_printf("[%c] AT+CESQ\n", is_success ? 'D' : 'F');
            // 参见 feedback_message_enum_t::bc26_send_at_cesq。
            fmq.post_message(
                _fmq_e_t::bc26_send_at_cesq,
                std::make_shared<std::tuple<bool, int>>(is_success, intensity));
        }
        void on_send_at_cesq()
        {
            on_send_at_cesq(_external_fmq);
        }

        /**
         * @brief 综合地初始化。
         *
         * @param max_retry 最大重试次数。
         */
        void on_init(int max_retry, _fmq_t& fmq)
        {
            _fmq_t internal_fmq;
            _fmq_t::message_t msg;
            bool any_success = false;
            std::string card_id;
            bool is_activated;
            int intensity;

            on_software_reset(internal_fmq);
            msg = internal_fmq.get_message();
            assert(msg.first == _fmq_e_t::bc26_software_reset);

            for (int i = 0; i < max_retry; i++)
            {
                bool once_success = false;
                using namespace std::literals;
                do
                {
                    on_send_at(10, internal_fmq);
                    msg = internal_fmq.get_message();
                    assert(msg.first == _fmq_e_t::bc26_send_at);
                    if (!utils::msg_data<bool>(msg))
                        break;

                    on_send_ate(false, internal_fmq);
                    msg = internal_fmq.get_message();
                    assert(msg.first == _fmq_e_t::bc26_send_ate);
                    if (!utils::msg_data<bool>(msg))
                        break;

                    on_send_at_cfun_set(1, internal_fmq);
                    msg = internal_fmq.get_message();
                    assert(msg.first == _fmq_e_t::bc26_send_at_cfun_set);
                    if (!utils::msg_data<bool>(msg))
                        break;

                    on_send_at_cimi(internal_fmq);
                    msg = internal_fmq.get_message();
                    assert(msg.first == _fmq_e_t::bc26_send_at_cimi);
                    {
                        const auto& data = *std::static_pointer_cast<
                            std::tuple<bool, std::string>>(msg.second);

                        if (!std::get<0>(data))
                            break;
                        card_id = std::get<1>(data);
                    }

                    on_send_at_cgatt_get(internal_fmq);
                    msg = internal_fmq.get_message();
                    assert(msg.first == _fmq_e_t::bc26_send_at_cgatt_get);
                    {
                        const auto& data =
                            *std::static_pointer_cast<std::tuple<bool, bool>>(
                                msg.second);

                        if (!std::get<0>(data))
                            break;
                        is_activated = std::get<1>(data);
                    }

                    on_send_at_cesq(internal_fmq);
                    msg = internal_fmq.get_message();
                    assert(msg.first == _fmq_e_t::bc26_send_at_cesq);
                    {
                        const auto& data =
                            *std::static_pointer_cast<std::tuple<bool, int>>(
                                msg.second);

                        if (!std::get<0>(data))
                            break;
                        intensity = std::get<1>(data);
                    }

                    once_success = true;
                } while (false);
                if (once_success)
                {
                    any_success = true;
                    break;
                }

                // 等待 5 s，保证之后初始化成功。
                if (i + 1 != max_retry)
                    rtos::ThisThread::sleep_for(5s);
            }

            fmq.post_message(
                _fmq_e_t::bc26_init,
                std::make_shared<std::tuple<bool, std::string, bool, int>>(
                    any_success, card_id, is_activated, intensity));
        }
        void on_init(int max_retry)
        {
            on_init(max_retry, _external_fmq);
        }

        /**
         * @brief 发送 AT+QIOPEN= 指令。打开 Socket 服务。
         *
         * @todo 测试该功能。
         *
         * @param address 远程服务器的 IP 地址或域名地址。不包含引号。
         * @param remote_port 远程服务器的端口号。范围 1-65535。
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         * @param is_service_type_tcp Socket 服务类型是否为 TCP。
         * false 表示服务类型为 UDP。
         */
        void on_send_at_qiopen(const std::string& address, int remote_port,
                               int connect_id, bool is_service_type_tcp,
                               _fmq_t& fmq)
        {
            std::string cmd = "AT+QIOPEN=";
            cmd += "1"; // 场景 ID，目前只能为 1。
            assert(0 <= connect_id && connect_id <= 4);
            cmd += "," + std::to_string(connect_id);
            if (is_service_type_tcp)
                cmd += ",\"TCP\"";
            else
                cmd += ",\"UDP\"";
            cmd += ",\"" + address + "\"";
            assert(1 <= remote_port && remote_port <= 65535);
            cmd += "," + std::to_string(remote_port);
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str;
            // 至多会等待 60s。
            // TODO: 增加等待超时。目前假设总是会成功。
            do
            {
                received_str = receiver.receive_command(300ms);
            } while (received_str.find("+QIOPEN:") == std::string::npos);
            // 额外再收一次，确保收完。
            received_str += receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            int returned_connect_id{};
            int result{};
            if (is_success &&
                2 != sscanf(received_str.c_str(), "OK\r\n\r\n+QIOPEN: %d,%d",
                            &returned_connect_id, &result))
                is_success = false;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qiopen。
            fmq.post_message(_fmq_e_t::bc26_send_at_qiopen,
                             std::make_shared<std::tuple<bool, int, int>>(
                                 is_success, returned_connect_id, result));
        }
        void on_send_at_qiopen(const std::string& address, int remote_port,
                               int connect_id, bool is_service_type_tcp)
        {
            on_send_at_qiopen(address, remote_port, connect_id,
                              is_service_type_tcp, _external_fmq);
        }
        /**
         * @brief 发送 AT+QICLOSE= 指令。关闭 Socket 服务。
         *
         * @todo 测试该功能。
         *
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         */
        void on_send_at_qiclose(int connect_id, _fmq_t& fmq)
        {
            std::string cmd = "AT+QICLOSE=";
            assert(0 <= connect_id && connect_id <= 4);
            cmd += std::to_string(connect_id);
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success =
                received_str.find("CLOSE OK") != std::string::npos;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qiclose。
            fmq.post_message(_fmq_e_t::bc26_send_at_qiclose,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at_qiclose(int connect_id)
        {
            on_send_at_qiclose(connect_id, _external_fmq);
        }
        /**
         * @brief 发送 AT+QISEND= 指令。发送文本字符串数据。
         *
         * @todo 测试该功能。
         *
         * @param str 要发送的文本字符串。
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         */
        void on_send_at_qisend(const std::string& str, int connect_id,
                               _fmq_t& fmq)
        {
            std::string cmd = "AT+QISEND=";
            assert(0 <= connect_id && connect_id <= 4);
            cmd += std::to_string(connect_id);
            assert(str.length() <= 1024);
            cmd += "," + std::to_string(str.length());
            cmd += ",\"" + str + "\"";
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            if (is_success)
                is_success &= received_str.find("SEND OK") != std::string::npos;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qisend。
            fmq.post_message(_fmq_e_t::bc26_send_at_qisend,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at_qisend(const std::string& str, int connect_id)
        {
            on_send_at_qisend(str, connect_id, _external_fmq);
        }
        /**
         * @brief 发送 AT+QIRD= 指令。读取收到的 TCP/IP 数据。
         *
         * @todo 测试该功能。
         *
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         */
        void on_send_at_qird(int connect_id, _fmq_t& fmq)
        {
            // 考虑到串口缓冲区的默认大小为 256。
            constexpr int buffer_size = 128;

            std::string cmd = "AT+QIRD=";
            assert(0 <= connect_id && connect_id <= 4);
            cmd += std::to_string(connect_id);
            cmd += "," + std::to_string(buffer_size);
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            std::string data_read;
            // 提取 +QIRD: 后的那一行。
            if (is_success)
            {
                std::string_view raw{received_str};
                size_t line_start = 0;
                bool is_data = false;
                for (size_t i = 0; i < raw.size(); i++)
                {
                    if (raw[i] == '\r')
                        continue;
                    if (raw[i] == '\n')
                    {
                        if (is_data)
                        {
                            while (i && (raw[i] == '\r' || raw[i] == '\n'))
                                i--;
                            if (~i)
                            {
                                data_read = std::move(std::string(
                                    raw.substr(line_start, i - line_start)));
                            }
                            break;
                        }
                        else if (raw.substr(line_start, i - line_start)
                                     .substr(0, 6) == "+QIRD:")
                        {
                            is_data = true;
                        }
                        line_start = i + 1;
                    }
                }
            }

            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qird。
            fmq.post_message(_fmq_e_t::bc26_send_at_qird,
                             std::make_shared<std::tuple<bool, std::string>>(
                                 is_success, data_read));
        }
        void on_send_at_qird(int connect_id)
        {
            on_send_at_qird(connect_id, _external_fmq);
        }

        /**
         * @brief 发送 AT+QMTCFG= 指令。配置 MQTT 可选参数。
         *
         * @param type 类型。不包含引号。例如 dataformat。
         * @param params 参数列表。各参数将会被逗号隔开，需要手动添加引号。
         */
        void on_send_at_qmtcfg(const std::string& type,
                               const std::vector<std::string>& params,
                               _fmq_t& fmq)
        {
            std::string cmd = "AT+QMTCFG=";
            cmd += '"' + type + '"';
            for (const auto& param : params)
                cmd += ',' + param;
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qmtcfg。
            fmq.post_message(_fmq_e_t::bc26_send_at_qmtcfg,
                             std::make_shared<bool>(is_success));
        }
        void on_send_at_qmtcfg(const std::string& type,
                               const std::vector<std::string>& params)
        {
            on_send_at_qmtcfg(type, params, _external_fmq);
        }
        /**
         * @brief 发送 AT+QMTOPEN= 指令。打开 MQTT 客户端网络。
         *
         * @todo 测试该功能。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         * @param host_name 服务器地址，可以是 IP 地址或者域名。最大长度 100
         * 字节。不包含引号。
         * @param port 服务器端口。范围 1-65535。
         */
        void on_send_at_qmtopen(int tcp_connect_id,
                                const std::string& host_name, int port,
                                _fmq_t& fmq)
        {
            std::string cmd = "AT+QMTOPEN=";
            assert(0 <= tcp_connect_id && tcp_connect_id <= 5);
            cmd += std::to_string(tcp_connect_id);
            cmd += ",\"" + host_name + "\",";
            cmd += std::to_string(port);
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str;
            // 至多会等待 75s。
            // TODO: 增加等待超时。目前假设总是会成功。
            do
            {
                received_str = receiver.receive_command(300ms);
            } while (received_str.empty());
            // 额外再收一次，确保收完。
            received_str += receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            int returned_tcp_connect_id{};
            int result{};
            if (is_success &&
                2 != sscanf(received_str.c_str(), "OK\r\n\r\n+QMTOPEN: %d,%d",
                            &returned_tcp_connect_id, &result))
                is_success = false;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qmtopen。
            fmq.post_message(_fmq_e_t::bc26_send_at_qmtopen,
                             std::make_shared<std::tuple<bool, int, int>>(
                                 is_success, returned_tcp_connect_id, result));
        }
        void on_send_at_qmtopen(int tcp_connect_id,
                                const std::string& host_name, int port)
        {
            on_send_at_qmtopen(tcp_connect_id, host_name, port, _external_fmq);
        }
        /**
         * @brief 发送 AT+QMTCLOSE= 指令。关闭 MQTT 客户端网络。
         *
         * @todo 测试该功能。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         */
        void on_send_at_qmtclose(int tcp_connect_id, _fmq_t& fmq)
        {
            assert(0 <= tcp_connect_id && tcp_connect_id <= 5);
            std::string cmd =
                "AT+QMTCLOSE=" + std::to_string(tcp_connect_id) + "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            int returned_tcp_connect_id{};
            int result{};
            if (is_success &&
                2 != sscanf(received_str.c_str(), "OK\r\n\r\n+QMTCLOSE: %d,%d",
                            &returned_tcp_connect_id, &result))
                is_success = false;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qmtclose。
            fmq.post_message(_fmq_e_t::bc26_send_at_qmtclose,
                             std::make_shared<std::tuple<bool, int, int>>(
                                 is_success, returned_tcp_connect_id, result));
        }
        void on_send_at_qmtclose(int tcp_connect_id)
        {
            on_send_at_qmtclose(tcp_connect_id, _external_fmq);
        }
        /**
         * @brief 发送 AT+QMTCONN= 指令。客户端连接 MQTT 服务器。
         *
         * @todo 测试该功能。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         * @param client_id 客户端标识符。不包含引号。
         * @param username 客户端用户名，可用来鉴权。不包含引号。
         * @param password 客户端用户名对应的密码，可用来鉴权。不包含引号。
         */
        void on_send_at_qmtconn(int tcp_connect_id,
                                const std::string& client_id,
                                const std::string& username,
                                const std::string& password, _fmq_t& fmq)
        {
            std::string cmd = "AT+QMTCONN=";
            assert(0 <= tcp_connect_id && tcp_connect_id <= 5);
            cmd += std::to_string(tcp_connect_id);
            cmd += ",\"" + client_id + "\"";
            cmd += ",\"" + username + "\"";
            cmd += ",\"" + password + "\"";
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str;
            // 默认至多会等待 10s。
            // TODO: 增加等待超时。目前假设总是会成功。
            do
            {
                received_str = receiver.receive_command(300ms);
            } while (received_str.empty());
            // 额外再收一次，确保收完。
            received_str += receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            int returned_tcp_connect_id{};
            int result{};
            int ret_code{};
            if (is_success &&
                2 > sscanf(received_str.c_str(), "OK\r\n\r\n+QMTCONN: %d,%d,%d",
                           &returned_tcp_connect_id, &result, &ret_code))
                is_success = false;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qmtconn。
            fmq.post_message(
                _fmq_e_t::bc26_send_at_qmtconn,
                std::make_shared<std::tuple<bool, int, int, int>>(
                    is_success, returned_tcp_connect_id, result, ret_code));
        }
        void on_send_at_qmtconn(int tcp_connect_id,
                                const std::string& client_id,
                                const std::string& username,
                                const std::string& password)
        {
            on_send_at_qmtconn(tcp_connect_id, client_id, username, password,
                               _external_fmq);
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QMTDISC= 指令。MQTT
         * 服务器断开与客户端连接。
         *
         * @todo 测试该功能。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         */
        void on_send_at_qmtdisc(int tcp_connect_id, _fmq_t& fmq)
        {
            assert(0 <= tcp_connect_id && tcp_connect_id <= 5);
            std::string cmd =
                "AT+QMTDISC=" + std::to_string(tcp_connect_id) + "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str = receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            int returned_tcp_connect_id{};
            int result{};
            if (is_success &&
                2 != sscanf(received_str.c_str(), "OK\r\n\r\n+QMTDISC: %d,%d",
                            &returned_tcp_connect_id, &result))
                is_success = false;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qmtdisc。
            fmq.post_message(_fmq_e_t::bc26_send_at_qmtdisc,
                             std::make_shared<std::tuple<bool, int, int>>(
                                 is_success, returned_tcp_connect_id, result));
        }
        void on_send_at_qmtdisc(int tcp_connect_id)
        {
            on_send_at_qmtdisc(tcp_connect_id, _external_fmq);
        }
        /**
         * @brief 发送 AT+QMTSUB= 指令。订阅主题。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         * @param msg_id 数据包标识符，范围：1-65535。
         * @param topic 客户端想要订阅或者退订的主题，最大长度是 255 字节。
         * @param qos 客户端想要发送消息的 QoS 等级。
         * - 0 最多发送一次。
         * - 1 至少发送一次。
         * - 2 只发送一次。
         */
        void on_send_at_qmtsub(int tcp_connect_id, int msg_id,
                               const std::string& topic, int qos, _fmq_t& fmq)
        {
            std::string cmd = "AT+QMTSUB=";
            assert(0 <= tcp_connect_id && tcp_connect_id <= 5);
            cmd += std::to_string(tcp_connect_id);
            assert(0 <= msg_id && msg_id <= 65535);
            cmd += "," + std::to_string(msg_id);
            cmd += ",\"" + topic + "\"";
            cmd += "," + std::to_string(qos);
            cmd += "\r\n";

            utils::debug_printf("[-] %s", cmd.c_str());
            sender.send_command(cmd);
            std::string received_str;
            // 默认至多会等待 40s。
            // TODO: 增加等待超时。目前假设总是会成功。
            do
            {
                received_str = receiver.receive_command(300ms);
            } while (received_str.empty());
            // 额外再收一次，确保收完。
            received_str += receiver.receive_command(300ms);
            utils::debug_printf("%s", received_str.c_str());

            bool is_success = received_str.find("OK") != std::string::npos;
            int returned_tcp_connect_id{};
            int returned_msg_id{};
            int result{};
            int value{};
            if (is_success && 3 > sscanf(received_str.c_str(),
                                         "OK\r\n\r\n+QMTSUB: %d,%d,%d,%d",
                                         &returned_tcp_connect_id,
                                         &returned_msg_id, &result, &value))
                is_success = false;
            utils::debug_printf("[%c] %s", is_success ? 'D' : 'F', cmd.c_str());
            // 参见 feedback_message_enum_t::bc26_send_at_qmtsub。
            fmq.post_message(
                _fmq_e_t::bc26_send_at_qmtsub,
                std::make_shared<std::tuple<bool, int, int, int, int>>(
                    is_success, returned_tcp_connect_id, returned_msg_id,
                    result, value));
        }
        void on_send_at_qmtsub(int tcp_connect_id, int msg_id,
                               const std::string& topic, int qos)
        {
            on_send_at_qmtsub(tcp_connect_id, msg_id, topic, qos,
                              _external_fmq);
        }

        // 以下函数是主模块的接口，均在主线程中运行。
    public:
        /**
         * @brief 向子模块发送消息。重复发送 AT 指令，直到收到 OK。
         *
         * @param max_retry 最大重试次数。
         */
        void send_at(int max_retry = 10)
        {
            post_message(static_cast<int>(bc26_message_t::send_at),
                         std::make_shared<int>(max_retry));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QRST=1 指令。软件重置。
         */
        void software_reset()
        {
            post_message(static_cast<int>(bc26_message_t::software_reset),
                         nullptr);
        }
        /**
         * @brief 向子模块发送消息。发送 ATE<echo> 指令。打开或关闭回显。
         *
         * @param is_echo 是否打开回显。默认为不打开。
         */
        void send_ate(bool is_echo = false)
        {
            post_message(static_cast<int>(bc26_message_t::send_ate),
                         std::make_shared<bool>(is_echo));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+CFUN=<mode> 指令。设置功能模式。
         *
         * @param mode 功能模式。默认为 1。
         */
        void send_at_cfun_set(int mode = 1)
        {
            post_message(static_cast<int>(bc26_message_t::send_at_cfun_set),
                         std::make_shared<int>(mode));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+CIMI 指令。查询卡号。
         */
        void send_at_cimi()
        {
            post_message(static_cast<int>(bc26_message_t::send_at_cimi),
                         nullptr);
        }
        /**
         * @brief 向子模块发送消息。发送 AT_CGATT? 指令。查询激活状态。
         */
        void send_at_cgatt_get()
        {
            post_message(static_cast<int>(bc26_message_t::send_at_cgatt_get),
                         nullptr);
        }
        /**
         * @brief 向子模块发送消息。发送 AT+CESQ 指令。获取信号质量。
         */
        void send_at_cesq()
        {
            post_message(static_cast<int>(bc26_message_t::send_at_cesq),
                         nullptr);
        }

        /**
         * @brief 综合地初始化。
         *
         * @param max_retry 最大重试次数。
         */
        void init(int max_retry = 5)
        {
            post_message(static_cast<int>(bc26_message_t::init),
                         std::make_shared<int>(max_retry));
        }

        /**
         * @brief 向子模块发送消息。发送 AT+QIOPEN= 指令。打开 Socket 服务。
         *
         * @param address 远程服务器的 IP 地址或域名地址。不包含引号。
         * @param remote_port 远程服务器的端口号。范围 1-65535。
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         * @param is_service_type_tcp Socket 服务类型是否为 TCP。
         * false 表示服务类型为 UDP。
         */
        void send_at_qiopen(const std::string& address, int remote_port,
                            int connect_id = 0, bool is_service_type_tcp = true)
        {
            using param_type = std::tuple<std::string, int, int, bool>;
            post_message(static_cast<int>(bc26_message_t::send_at_qiopen),
                         std::make_shared<param_type>(address, remote_port,
                                                      connect_id,
                                                      is_service_type_tcp));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QICLOSE= 指令。关闭 Socket 服务。
         *
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         */
        void send_at_qiclose(int connect_id = 0)
        {
            post_message(static_cast<int>(bc26_message_t::send_at_qiclose),
                         std::make_shared<int>(connect_id));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QISEND= 指令。发送文本字符串数据。
         *
         * @param str 要发送的文本字符串。
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         */
        void send_at_qisend(const std::string& str, int connect_id = 0)
        {
            using param_type = std::tuple<std::string, int>;
            post_message(static_cast<int>(bc26_message_t::send_at_qisend),
                         std::make_shared<param_type>(str, connect_id));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QIRD= 指令。读取收到的 TCP/IP 数据。
         *
         * @param connect_id Socket 服务索引。范围 0-4。默认为 0。
         */
        void send_at_qird(int connect_id = 0)
        {
            post_message(static_cast<int>(bc26_message_t::send_at_qird),
                         std::make_shared<int>(connect_id));
        }

    private:
        /**
         * @brief 向子模块发送消息。发送 AT+QMTCFG= 指令。配置 MQTT 可选参数。
         *
         * @param type 类型。不包含引号。例如 dataformat。
         * @param params 参数列表。各参数将会被逗号隔开，需要手动添加引号。
         */
        void send_at_qmtcfg(const std::string& type,
                            const std::vector<std::string>& params)
        {
            using param_type =
                std::tuple<std::string, std::vector<std::string>>;
            post_message(static_cast<int>(bc26_message_t::send_at_qmtcfg),
                         std::make_shared<param_type>(type, params));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QMTOPEN= 指令。打开 MQTT
         * 客户端网络。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         * @param host_name 服务器地址，可以是 IP 地址或者域名。最大长度 100
         * 字节。不包含引号。
         * @param port 服务器端口。范围 1-65535。
         */
        void send_at_qmtopen(int tcp_connect_id, const std::string& host_name,
                             int port)
        {
            using param_type = std::tuple<int, std::string, int>;
            post_message(
                static_cast<int>(bc26_message_t::send_at_qmtopen),
                std::make_shared<param_type>(tcp_connect_id, host_name, port));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QMTCLOSE= 指令。关闭 MQTT
         * 客户端网络。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         */
        void send_at_qmtclose(int tcp_connect_id)
        {
            post_message(static_cast<int>(bc26_message_t::send_at_qmtclose),
                         std::make_shared<int>(tcp_connect_id));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QMTCONN= 指令。客户端连接 MQTT
         * 服务器。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         * @param client_id 客户端标识符。不包含引号。
         * @param username 客户端用户名，可用来鉴权。不包含引号。
         * @param password 客户端用户名对应的密码，可用来鉴权。不包含引号。
         */
        void send_at_qmtconn(int tcp_connect_id, const std::string& client_id,
                             const std::string& username,
                             const std::string& password)
        {
            using param_type =
                std::tuple<int, std::string, std::string, std::string>;
            post_message(static_cast<int>(bc26_message_t::send_at_qmtconn),
                         std::make_shared<param_type>(tcp_connect_id, client_id,
                                                      username, password));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QMTDISC= 指令。MQTT
         * 服务器断开与客户端连接。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         */
        void send_at_qmtdisc(int tcp_connect_id)
        {
            post_message(static_cast<int>(bc26_message_t::send_at_qmtdisc),
                         std::make_shared<int>(tcp_connect_id));
        }
        /**
         * @brief 向子模块发送消息。发送 AT+QMTSUB= 指令。订阅主题。
         *
         * @param tcp_connect_id MQTT Socket 标识符。范围 0-5。
         * @param msg_id 数据包标识符，范围：1-65535。
         * @param topic 客户端想要订阅或者退订的主题，最大长度是 255 字节。
         * @param qos 客户端想要发送消息的 QoS 等级。
         * - 0 最多发送一次。
         * - 1 至少发送一次。
         * - 2 只发送一次。
         */
        void send_at_qmtsub(int tcp_connect_id, int msg_id,
                            const std::string& topic, int qos)
        {
            using param_type = std::tuple<int, int, std::string, int>;
            post_message(static_cast<int>(bc26_message_t::send_at_qmtsub),
                         std::make_shared<param_type>(tcp_connect_id, msg_id,
                                                      topic, qos));
        }
    };
} // namespace peripheral
