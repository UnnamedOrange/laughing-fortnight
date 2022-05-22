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

#include <peripheral/bc26/bc26.hpp>
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

            while (true)
            {
            }
        }
    };
} // namespace test
