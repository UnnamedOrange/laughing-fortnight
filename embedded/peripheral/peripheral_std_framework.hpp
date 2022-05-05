/**
 * @file peripheral_std_framework.hpp
 * @author UnnamedOrange
 * @brief 外设子模块的标准框架。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <functional>
#include <memory>
#include <queue>
#include <type_traits>
#include <utility>

#include "peripheral_thread.hpp"

namespace peripheral
{
    /**
     * @brief 外设子模块的标准框架。是一个带有消息队列的子线程。
     */
    class peripheral_std_framework : public peripheral_thread
    {
    private:
        /**
         * @brief 消息队列的互斥体。
         */
        rtos::Mutex _mutex_queue;
        /**
         * @brief 消息队列的条件变量。
         */
        rtos::ConditionVariable _cond_queue{_mutex_queue};
        /**
         * @brief 消息队列。first 是 id，second 是 data。
         */
        std::queue<std::pair<int, std::shared_ptr<void>>> _queue;
        /**
         * @brief 子线程是否需要退出。实际上不可能退出，仅在测试时使用。
         */
        bool _should_exit{};

    public:
        ~peripheral_std_framework()
        {
            // 实际使用中该析构函数不会被执行，仅为了让测试正常进行。

            // 通知子线程退出。
            {
                rtos::ScopedMutexLock lock(_mutex_queue);
                _should_exit = true;
                _cond_queue.notify_one();
            }
            // 等待 1s，确保子线程退出。
            rtos::ThisThread::sleep_for(1s);
        }

    private:
        void thread_main() override
        {
            while (true)
            {
                std::decay<decltype(_queue.front())>::type message;
                {
                    // 信号接收流程：获取锁，检查条件表达式，满足条件时发送信号，释放锁。

                    // 获取锁。
                    rtos::ScopedMutexLock lock(_mutex_queue);
                    // 检查条件表达式。检查时锁会被放开，线程进入等待条件变量的状态。
                    _cond_queue.wait(
                        [this]() { return _should_exit || !_queue.empty(); });
                    // 需要退出时直接退出。实际中不会发生，只是为了让测试正常进行。
                    if (_should_exit)
                        break;
                    // 等待结束后，线程获得锁，仅子线程访问队列。
                    message = _queue.front();
                    _queue.pop();
                    // ScopedMutexLock 析构函数自动释放锁。
                }

                // 在子线程中处理消息。此时队列锁已释放。
                on_message(message.first, std::move(message.second));
            }
        }

    protected:
        /**
         * @brief 待实现的纯虚函数，表示消息处理程序。
         *
         * @note 该函数在子线程中运行。
         *
         * @param id 程序员自定义的消息 ID。
         * @param data 指向程序员自定义数据结构的指针。
         */
        virtual void on_message(int id, std::shared_ptr<void> data) = 0;

    public:
        /**
         * @brief 向消息队列中添加消息。
         *
         * @note 在主线程中调用该函数。也可以在子线程内部调用该函数。
         *
         * @param id 程序员自定义的消息 ID。
         * @param data 指向程序员自定义数据结构的指针。
         */
        void push(int id, std::shared_ptr<void> data)
        {
            // 信号发送流程：获取锁，检查条件表达式，满足条件时发送信号，释放锁。

            // 获取锁。
            rtos::ScopedMutexLock lock(_mutex_queue);
            // 加锁后，仅主线程访问队列。shared_ptr 的引用计数是线程安全的。
            _queue.push(std::make_pair(id, std::move(data)));
            // 发送信号。
            _cond_queue.notify_one();
            // ScopedMutexLock 析构函数自动释放锁。
        }
        /**
         * @brief 消息队列是否为空。
         *
         * @note 该函数是线程安全的。
         */
        bool empty()
        {
            rtos::ScopedMutexLock lock(_mutex_queue);
            return _queue.empty();
        }
    };
} // namespace peripheral
