/**
 * @file message_queue.hpp
 * @author UnnamedOrange
 * @brief 多生产者单消费者的消息队列。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <deque>
#include <utility>

namespace peripheral
{
    /**
     * @brief 多生产者单消费者的消息队列。
     *
     * @note 这个类是线程安全的。
     */
    class message_queue
    {
    public:
        /**
         * @brief 消息。
         *
         * @param first id。
         * @param second data。
         */
        using raw_message_t = std::pair<int, std::shared_ptr<void>>;

    private:
        // 消息队列的互斥体。
        rtos::Mutex _mutex_queue;
        // 消息队列的条件变量。
        rtos::ConditionVariable _cond_queue{_mutex_queue};
        // 消息队列。
        std::deque<raw_message_t> _queue;
        // 是否需要退出。
        bool _should_exit{};

    protected:
        void exit()
        {
            _should_exit = true;
            // 等待消息队列不再被访问。
            rtos::ScopedMutexLock lock{_mutex_queue};
            // 通知等待中的条件变量退出。
            _cond_queue.notify_one();
        }

    public:
        virtual ~message_queue()
        {
            exit();
        }

    public:
        /**
         * @brief 消息队列是否为空。
         */
        bool empty()
        {
            if (_should_exit)
                return true;

            rtos::ScopedMutexLock lock{_mutex_queue};
            return _queue.empty();
        }

    public:
        /**
         * @brief 向消息队列发送消息。
         *
         * @param id 消息 id。0 表示退出，不要发送 0。
         * @param data 消息的额外数据。
         */
        void post_message(int id, std::shared_ptr<void> data)
        {
            if (_should_exit)
                return;

            rtos::ScopedMutexLock lock{_mutex_queue};
            _queue.push_back({id, data});
            _cond_queue.notify_one();
        }
        /**
         * @brief 向消息队列发送消息。
         * 如果这种类型的消息已经存在，则覆盖最晚的消息。
         *
         * @param id 消息 id。0 表示退出，不要发送 0。
         * @param data 消息的额外数据。
         */
        void post_message_unique(int id, std::shared_ptr<void> data)
        {
            if (_should_exit)
                return;

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
         * @return raw_message_t 收到的消息。
         */
        raw_message_t get_message()
        {
            if (_should_exit)
                return {0, nullptr};

            raw_message_t message;
            {
                rtos::ScopedMutexLock lock(_mutex_queue);
                _cond_queue.wait(
                    [this]() { return _should_exit || !_queue.empty(); });
                if (_should_exit)
                    return {0, nullptr};
                message = _queue.front();
                _queue.pop_front();
            }
            return message;
        }
        /**
         * @brief 阻塞地获取消息队列中的消息。如果队列中没有范围内的消息则等待。
         *
         * @param min_message 获取消息的最小编号。
         * @param max_message 获取消息的最大编号。
         * @return raw_message_t 收到的消息。
         */
        raw_message_t get_message(int min_message, int max_message)
        {
            if (_should_exit)
                return {0, nullptr};

            raw_message_t message;
            {
                rtos::ScopedMutexLock lock(_mutex_queue);
                _cond_queue.wait([this, &min_message, &max_message]() {
                    return _should_exit ||
                           std::count_if(
                               _queue.rbegin(),
                               _queue.rend(), // 倒序以加快新消息的查找。
                               [&](const raw_message_t& msg) {
                                   return min_message <= msg.first &&
                                          msg.first <= max_message;
                               });
                });
                if (_should_exit)
                    return {0, nullptr};
                // 顺序查找最早的消息。
                for (auto it = _queue.begin(); it != _queue.end(); it++)
                {
                    if (min_message <= it->first && it->first <= max_message)
                    {
                        message = *it;
                        _queue.erase(it);
                        break;
                    }
                }
            }
            return message;
        }
        /**
         * @brief 非阻塞地获取消息队列中的消息。
         *
         * @return raw_message_t 收到的消息。
         * 如果队列为空，返回 {0, nullptr}。
         */
        raw_message_t peek_message()
        {
            if (_should_exit)
                return {0, nullptr};

            rtos::ScopedMutexLock lock{_mutex_queue};
            if (_queue.empty())
                return {0, nullptr};
            auto message = _queue.front();
            _queue.pop_front();
            return message;
        }
        /**
         * @brief 非阻塞地获取消息队列中的消息。
         *
         * @param min_message 获取消息的最小编号。
         * @param max_message 获取消息的最大编号。
         * @return raw_message_t 收到的消息。如果队列中没有范围内的消息，
         * 返回 {0, nullptr}。
         */
        raw_message_t peek_message(int min_message, int max_message)
        {
            if (_should_exit)
                return {0, nullptr};

            rtos::ScopedMutexLock lock{_mutex_queue};
            raw_message_t message{0, nullptr};
            // 顺序查找最早的消息。
            for (auto it = _queue.begin(); it != _queue.end(); it++)
            {
                if (min_message <= it->first && it->first <= max_message)
                {
                    message = *it;
                    _queue.erase(it);
                    break;
                }
            }
            return message;
        }
    };
} // namespace peripheral
