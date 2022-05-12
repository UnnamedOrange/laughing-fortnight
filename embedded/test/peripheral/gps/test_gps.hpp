/**
 * @file test_gps.hpp
 * @author UnnamedOrange
 * @brief 测试 GPS 模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>
#include <memory>

#include <peripheral/feedback_message.hpp>
#include <peripheral/feedback_message_queue.hpp>
#include <peripheral/gps/gps.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>
#include <utils/msg_data.hpp>

namespace test
{
    /**
     * @brief 测试 GPS 模块。
     *
     * @note 该测试是阻塞的，所以没有包含在 test_all 中。
     */
    class test_gps
    {
        peripheral::feedback_message_queue fmq;
        peripheral::gps gps{fmq};

    public:
        test_gps()
        {
            using namespace std::literals;
            utils::debug_printf("\n");
            utils::debug_printf("[I] gps test.\n");

            // 初始化 GPS 模块。
            {
                utils::debug_printf("[-] Init gps.\n");
                gps.init();
            }

            // 不断阻塞地等待 GPS 传来请求。
            {
                int times = 0;
                while (true)
                {
                    auto msg = fmq.get_message();
                    using fmq_e_t = peripheral::feedback_message_enum_t;
                    switch (msg.first)
                    {
                    case fmq_e_t::gps_init:
                    {
                        bool is_success = utils::msg_data<bool>(msg);
                        if (is_success)
                            utils::debug_printf("[D] Init gps.\n");
                        else
                        {
                            utils::debug_printf("[F] Init gps.\n");
                            rtos::ThisThread::sleep_for(1s);
                            error("Init gps failed.");
                            return;
                        }
                        break;
                    }
                    default:
                    {
                        utils::debug_printf("[E] Unknown: %d\n",
                                            static_cast<int>(msg.first));
                        rtos::ThisThread::sleep_for(1s);
                        error("Unknown message %d.",
                              static_cast<int>(msg.first));
                        return;
                    }
                    }
                }
            }
        }
    };
} // namespace test
