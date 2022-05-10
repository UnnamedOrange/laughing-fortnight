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

#include <deque>

#include "feedback_message.hpp"

namespace peripheral
{
    /**
     * @brief 用于子模块向主模块反馈消息的消息队列。
     * 是一个多生产者单消费者的队列。
     *
     * @todo 测试这个类。
     *
     * @note 这个类是线程安全的。
     */
    class feedback_message_queue
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
        // 消息队列的互斥体。
        rtos::Mutex _mutex_queue;
        // 消息队列的条件变量。
        rtos::ConditionVariable _cond_queue{_mutex_queue};
        // 消息队列。
        std::deque<message_t> _queue;

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
            rtos::ScopedMutexLock lock{_mutex_queue};
            _queue.push_back({id, data});
            _cond_queue.notify_one();
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
            rtos::ScopedMutexLock lock{_mutex_queue};
            bool _has_found = false;
            // 倒序查找最晚的消息。
            for (auto it = _queue.rbegin(); it != _queue.rend(); it++)
            {
                if (it->first == id) // 找到了这种类型的消息。
                {
                    _has_found = true;
                    it->second = data; // 将其额外数据覆盖。
                    break;
                }
            }

            if (!_has_found) // 如果没有找到，正常发送。
                post_message(id, data);
        }

    public:
        /**
         * @brief 阻塞地获取消息队列中的消息。如果队列为空则等待。
         *
         * @return message_t 收到的消息。
         */
        message_t get_message()
        {
            message_t message;
            {
                rtos::ScopedMutexLock lock(_mutex_queue);
                _cond_queue.wait([this]() { return !_queue.empty(); });
                message = _queue.front();
                _queue.pop_front();
            }
            return message;
        }
        /**
         * @brief 非阻塞地获取消息队列中的消息。
         *
         * @return message_t 收到的消息。
         * 如果队列为空，返回 {feedback_message_enum_t::null, nullptr}。
         */
        message_t peek_message()
        {
            rtos::ScopedMutexLock lock{_mutex_queue};
            if (_queue.empty())
                return {feedback_message_enum_t::null, nullptr};
            auto message = _queue.front();
            _queue.pop_front();
            return message;
        }
    };
} // namespace peripheral
