/**
 * @file test_accel.hpp
 * @author UnnamedOrange
 * @brief 测试加速度计模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>
#include <memory>

#include <peripheral/accel/accel.hpp>
#include <peripheral/feedback_message.hpp>
#include <peripheral/feedback_message_queue.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>

namespace test
{
    /**
     * @brief 测试加速度计模块。
     *
     * @note 该测试是阻塞的，所以没有包含在 test_all 中。
     */
    class test_accel
    {
        peripheral::feedback_message_queue fmq;
        peripheral::accel accel{fmq};

    public:
        test_accel()
        {
            using namespace std::literals;
            utils::debug_printf("\n");
            utils::debug_printf("[I] accel test.\n");

            // 初始化加速度计模块。
            {
                utils::debug_printf("[-] Init accel.\n");
                accel.init();
            }

            // 不断阻塞地等待加速度计传来请求。
            {
                int times = 0;
                while (true)
                {
                    auto msg = fmq.get_message();
                    using fmq_e_t = peripheral::feedback_message_enum_t;
                    switch (msg.first)
                    {
                    case fmq_e_t::accel_init:
                    {
                        bool is_success =
                            *std::static_pointer_cast<bool>(msg.second);
                        if (is_success)
                            utils::debug_printf("[D] Init accel.\n");
                        else
                        {
                            utils::debug_printf("[F] Init accel.\n");
                            rtos::ThisThread::sleep_for(1s);
                            error("Init accel failed. Debug to probe.");
                            return;
                        }
                        break;
                    }
                    case fmq_e_t::accel_notify:
                    {
                        utils::debug_printf("accel notify %d\n", ++times);
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
