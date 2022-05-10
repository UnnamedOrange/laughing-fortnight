/**
 * @file test_feedback_message_queue.hpp
 * @author UnnamedOrange
 * @brief 测试 peripheral/feedback_message_queue.hpp。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <peripheral/feedback_message.hpp>
#include <peripheral/feedback_message_queue.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>

namespace test
{
    /**
     * @brief 测试 feedback_message_queue。
     * - 测试基本的 post, get, peek 功能。
     * - 测试 post_message_unique 功能。
     * - 测试带消息过滤的 get, peek 功能。
     */
    class test_feedback_message_queue
    {
    public:
        test_feedback_message_queue()
        {
            using namespace std::literals;
            utils::debug_printf("\n");
            utils::debug_printf("[I] feedback_message_queue test.\n");

            // 测试简单功能。
            {
                peripheral::feedback_message_queue q;

                q.post_message(peripheral::feedback_message_enum_t::bc26_init,
                               nullptr);

                // 测试 get_message 收正常消息。
                utils::debug_printf("[-] get_message\n");
                auto msg = q.get_message();
                // 没有阻塞就说明成功。
                utils::debug_printf("[D] get_message\n");
                rtos::ThisThread::sleep_for(1s);

                // 测试 peek_message 收空消息。
                utils::debug_printf("[-] peek_message\n");
                msg = q.peek_message();
                // 应该是空消息。
                if (msg.first == peripheral::feedback_message_enum_t::null)
                    utils::debug_printf("[D] peek_message\n");
                else
                    utils::debug_printf("[F] peek_message\n");
                rtos::ThisThread::sleep_for(1s);
            }

            // 测试 post_message_unique。
            {
                peripheral::feedback_message_queue q;

                // 测试 post_message_unique 发正常消息。
                utils::debug_printf("[-] post unique 1\n");
                q.post_message_unique(
                    peripheral::feedback_message_enum_t::bc26_init, nullptr);
                auto msg = q.get_message();
                // 没有阻塞就说明成功。
                utils::debug_printf("[D] post unique 1\n");
                rtos::ThisThread::sleep_for(1s);

                // 测试 post_message_unique 是否能去重，额外数据是否正确。
                utils::debug_printf("[-] post unique 2\n");
                do
                {
                    for (int i = 0; i < 2; i++)
                        q.post_message_unique(
                            peripheral::feedback_message_enum_t::bc26_init,
                            std::make_shared<int>(i));

                    msg = q.peek_message();
                    // 消息类型不对或额外数据错误。额外数据应该是 1。
                    if (msg.first !=
                            peripheral::feedback_message_enum_t::bc26_init ||
                        *std::static_pointer_cast<int>(msg.second) != 1)
                    {
                        utils::debug_printf("[F] post unique 2\n");
                        break;
                    }

                    msg = q.peek_message();
                    // 应该是空消息。
                    if (msg.first == peripheral::feedback_message_enum_t::null)
                        utils::debug_printf("[D] post unique 2\n");
                    else
                        utils::debug_printf("[F] post unique 2\n");
                } while (false);
                rtos::ThisThread::sleep_for(1s);
            }

            // 测试消息过滤。
            {
                peripheral::feedback_message_queue q;

                q.post_message(peripheral::feedback_message_enum_t::bc26_init,
                               nullptr);

                // 测试 get_message 能否收到过滤范围内的消息。
                utils::debug_printf("[-] filter 1\n");
                q.post_message_unique(
                    peripheral::feedback_message_enum_t::bc26_init, nullptr);
                auto msg = q.get_message(
                    peripheral::feedback_message_enum_t::bc26_message_begin,
                    peripheral::feedback_message_enum_t::bc26_message_end);
                // 没有阻塞就说明成功。
                utils::debug_printf("[D] filter 1\n");
                rtos::ThisThread::sleep_for(1s);

                // 测试 peek_message 能否忽视过滤范围外的消息。
                utils::debug_printf("[-] filter 2\n");
                q.post_message(peripheral::feedback_message_enum_t::bc26_init,
                               nullptr);
                msg = q.peek_message(
                    peripheral::feedback_message_enum_t::accel_message_begin,
                    peripheral::feedback_message_enum_t::accel_message_end);
                // 应该是空消息。
                if (msg.first == peripheral::feedback_message_enum_t::null)
                    utils::debug_printf("[D] filter 2\n");
                else
                    utils::debug_printf("[F] filter 2\n");
                rtos::ThisThread::sleep_for(1s);

                // 测试 peek_message 能否收到过滤范围内的消息。
                utils::debug_printf("[-] filter 3\n");
                msg = q.peek_message(
                    peripheral::feedback_message_enum_t::bc26_message_begin,
                    peripheral::feedback_message_enum_t::bc26_message_end);
                // 应该是 bc26_init。
                if (msg.first == peripheral::feedback_message_enum_t::bc26_init)
                    utils::debug_printf("[D] filter 3\n");
                else
                    utils::debug_printf("[F] filter 3\n");
                rtos::ThisThread::sleep_for(1s);

                // 测试 get_message 能否忽视过滤范围外的消息。
                // 以下代码阻塞就说明成功。
                // utils::debug_printf("[-] filter 4\n");
                // q.post_message(peripheral::feedback_message_enum_t::bc26_init,
                //                nullptr);
                // msg = q.get_message(
                //     peripheral::feedback_message_enum_t::accel_message_begin,
                //     peripheral::feedback_message_enum_t::accel_message_end);
            }
        }
    };
} // namespace test
