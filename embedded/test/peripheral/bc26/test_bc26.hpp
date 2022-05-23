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

            // 打开 TCP 连接。
            if (init_success)
            {
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
                            rtos::ThisThread::sleep_for(500ms);
                            utils::debug_printf("[W] retry\n");
                        }
                    }
                    else
                    {
                        error("Unknown message %d.",
                              static_cast<int>(msg.first));
                    }
                } while (!init_success);
            }

            // 如果初始化失败，则一直阻塞。
            if (!init_success)
            {
                while (true)
                    sleep();
            }

            while (true)
            {
            }
        }
    };
} // namespace test
