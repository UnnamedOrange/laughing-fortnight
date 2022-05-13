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

#include <algorithm>
#include <deque>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "message_queue.hpp"
#include "peripheral_thread.hpp"

namespace peripheral
{
    /**
     * @brief 外设子模块的标准框架。是一个带有消息队列的子线程。
     */
    class peripheral_std_framework : public peripheral_thread,
                                     public message_queue
    {
    private:
        using message_queue::get_message;
        using message_queue::peek_message;

    public:
        peripheral_std_framework()
        {
            peripheral_thread::start();
        }
        ~peripheral_std_framework()
        {
            // 逻辑上退出消息队列，可以认为队列总是空。
            message_queue::exit();
            // 通知子线程退出。
            peripheral_thread::join();
        }

        // 解决子类的线程安全问题。
    private:
        bool _descendant_exit{};
        rtos::Mutex _mutex_descendant;

    protected:
        /**
         * @brief 在子类析构函数中调用该函数，通知父类不再处理新消息。
         * 这是为了防止子类被销毁后父类还准备执行新的消息。
         */
        void descendant_exit()
        {
            _descendant_exit = true;
            _mutex_descendant.lock();
        }
        /**
         * @brief 在子类消息处理函数开头调用该函数。
         */
        void descendant_callback_begin()
        {
            _mutex_descendant.lock();
        }
        /**
         * @brief 在子类消息处理函数末尾调用该函数。
         */
        void descendant_callback_end()
        {
            _mutex_descendant.unlock();
        }

    private:
        void thread_main() override
        {
            while (true)
            {
                auto message = get_message();
                if (!message.first) // 说明消息队列已退出。
                    break;

                // 如果子类已经退出，则不执行消息处理。
                if (_descendant_exit)
                    break;
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
    };
} // namespace peripheral
