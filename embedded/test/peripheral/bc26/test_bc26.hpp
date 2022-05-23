/**
 * @file test_bc26.hpp
 * @author UnnamedOrange
 * @brief 测试 BC26 模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>
#include <string>
#include <tuple>

#include <peripheral/bc26/bc26.hpp>
#include <peripheral/bc26/bc26_config.hpp>
#include <peripheral/feedback_message.hpp>
#include <peripheral/feedback_message_queue.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>
#include <utils/msg_data.hpp>

namespace test
{
    /**
     * @brief 测试 BC26 模块。
     *
     * @note 该测试是阻塞的，所以没有包含在 test_all 中。
     */
    class test_bc26
    {
        using fmq_e_t = peripheral::feedback_message_enum_t;
        peripheral::feedback_message_queue fmq;
        peripheral::bc26 bc26{fmq};

    public:
        test_bc26()
        {
            using namespace std::literals;
            utils::debug_printf("\n");
            utils::debug_printf("[I] bc26 test.\n");

            bool init_success = false;
            auto msg = fmq.peek_message();

            // 初始化 BC26 模块。
            {
                utils::debug_printf("[-] init.\n");
                bc26.init();
                msg = fmq.get_message();
                if (msg.first == fmq_e_t::bc26_init)
                {
                    using param_type = std::tuple<bool, string, bool, int>;
                    auto t = utils::msg_data<param_type>(msg);
                    if (std::get<0>(t))
                    {
                        utils::debug_printf("[D] init.\n");
                        init_success = true;
                    }
                    else
                        utils::debug_printf("[F] init.\n");
                }
                else
                {
                    error("Unknown message %d.", static_cast<int>(msg.first));
                }
            }

            auto open_tcp = [&]() {
                utils::debug_printf("[-] tcp open.\n");
                init_success = false;
                do
                {
                    bc26.send_at_qiopen(remote_address, remote_port);
                    msg = fmq.get_message();
                    if (msg.first == fmq_e_t::bc26_send_at_qiopen)
                    {
                        using param_type = std::tuple<bool, int, int>;
                        auto t = utils::msg_data<param_type>(msg);
                        if (std::get<0>(t) && !std::get<2>(t))
                        {
                            utils::debug_printf("[D] tcp open.\n");
                            init_success = true;
                            break; // 目前 break 可以省略。
                        }
                        else
                        {
                            bc26.send_at_qiclose();
                            msg = fmq.get_message();
                            if (msg.first == fmq_e_t::bc26_send_at_qiclose)
                            {
                                auto close_succuss = utils::msg_data<bool>(msg);
                                if (close_succuss)
                                    utils::debug_printf("[I] tcp close.\n");
                                else
                                    utils::debug_printf("[E] close fail.\n");
                            }
                            else
                            {
                                error("Unknown message %d.",
                                      static_cast<int>(msg.first));
                            }
                            rtos::ThisThread::sleep_for(5s);
                            utils::debug_printf("[W] retry\n");
                        }
                    }
                    else
                    {
                        error("Unknown message %d.",
                              static_cast<int>(msg.first));
                    }
                } while (!init_success);
            };

            // 打开 TCP 连接。
            if (init_success)
                open_tcp();

            // 如果初始化失败，则一直阻塞。
            if (!init_success)
            {
                while (true)
                    sleep();
            }

            // 之后每隔 50 s 向上传一次，每隔 5 s 查询一次。
            int round = 0;
            int count = 0;
            while (true)
            {
                // 查询。
                {
                    bc26.send_at_qird();
                }

                // 上传。
                if (count == 0)
                {
                    bc26.send_at_qisend("Current round: " +
                                        std::to_string(round));
                }

                count = count + 1;
                if (count >= 10)
                {
                    round++;
                    count = 0;
                }
                rtos::ThisThread::sleep_for(5s);

                // 处理消息。
                bool should_open_tcp = false;
                while (true)
                {
                    msg = fmq.peek_message();
                    if (!static_cast<bool>(msg.first))
                        break;

                    switch (msg.first)
                    {
                    case fmq_e_t::bc26_send_at_qird:
                    {
                        const auto& data =
                            utils::msg_data<std::tuple<bool, std::string>>(msg);
                        if (std::get<0>(data))
                        {
                            if (std::get<1>(data).length())
                            {
                                utils::debug_printf("[I] received.\n");
                                utils::debug_printf("data: %s\n",
                                                    std::get<1>(data).c_str());
                            }
                        }
                        else
                        {
                            utils::debug_printf("[E] qird fail.\n");
                            should_open_tcp = true;
                        }
                        break;
                    }
                    case fmq_e_t::bc26_send_at_qisend:
                    {
                        const auto& is_ok = utils::msg_data<bool>(msg);
                        if (!is_ok)
                            should_open_tcp = true;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                if (should_open_tcp)
                    open_tcp();
            }
        }
    };
} // namespace test
