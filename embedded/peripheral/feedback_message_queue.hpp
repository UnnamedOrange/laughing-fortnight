/**
 * @file feedback_message_queue.hpp
 * @author UnnamedOrange
 * @brief 用于子模块向主模块反馈消息的消息队列。
 * 是一个多生产者单消费者的队列。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <type_traits>

#include "feedback_message.hpp"
#include "message_queue.hpp"

namespace peripheral
{
    /**
     * @brief 用于子模块向主模块反馈消息的消息队列。
     * 是一个多生产者单消费者的队列。
     *
     * @note 这个类是线程安全的。
     */
    class feedback_message_queue : protected message_queue
    {
    public:
        /**
         * @brief 消息。
         *
         * @param first id。
         * @param second data。
         */
        using message_t =
            std::pair<feedback_message_enum_t, std::shared_ptr<void>>;

    private:
        static message_t raw_to_msg(raw_message_t&& raw)
        {
            return {static_cast<feedback_message_enum_t>(raw.first),
                    std::move(raw.second)};
        }

    public:
        /**
         * @brief 向消息队列发送消息。
         *
         * @param id 消息 id。
         * @param data 消息的额外数据。
         */
        void post_message(feedback_message_enum_t id,
                          std::shared_ptr<void> data)
        {
            message_queue::post_message(static_cast<int>(id), data);
        }
        /**
         * @brief 向消息队列发送消息。
         * 如果这种类型的消息已经存在，则覆盖最晚的消息。
         *
         * @param id 消息 id。
         * @param data 消息的额外数据。
         */
        void post_message_unique(feedback_message_enum_t id,
                                 std::shared_ptr<void> data)
        {
            message_queue::post_message_unique(static_cast<int>(id), data);
        }

    public:
        /**
         * @brief 阻塞地获取消息队列中的消息。如果队列为空则等待。
         *
         * @return message_t 收到的消息。
         */
        message_t get_message()
        {
            return raw_to_msg(message_queue::get_message());
        }
        /**
         * @brief 阻塞地获取消息队列中的消息。如果队列中没有范围内的消息则等待。
         *
         * @param min_message 获取消息的最小编号。
         * @param max_message 获取消息的最大编号。
         * @return message_t 收到的消息。
         */
        message_t get_message(feedback_message_enum_t min_message,
                              feedback_message_enum_t max_message)
        {
            return raw_to_msg(message_queue::get_message(
                static_cast<int>(min_message), static_cast<int>(max_message)));
        }
        /**
         * @brief 非阻塞地获取消息队列中的消息。
         *
         * @return message_t 收到的消息。
         * 如果队列为空，返回 {feedback_message_enum_t::null, nullptr}。
         */
        message_t peek_message()
        {
            return raw_to_msg(message_queue::peek_message());
        }
        /**
         * @brief 非阻塞地获取消息队列中的消息。
         *
         * @param min_message 获取消息的最小编号。
         * @param max_message 获取消息的最大编号。
         * @return message_t 收到的消息。如果队列中没有范围内的消息，
         * 返回 {feedback_message_enum_t::null, nullptr}。
         */
        message_t peek_message(feedback_message_enum_t min_message,
                               feedback_message_enum_t max_message)
        {
            return raw_to_msg(message_queue::peek_message(
                static_cast<int>(min_message), static_cast<int>(max_message)));
        }

    public:
        using message_queue::empty;
    };
} // namespace peripheral
