/**
 * @file peripheral_thread.hpp
 * @author UnnamedOrange
 * @brief 在单独线程中运行的外设模块基类。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <functional>

#include <utils/debug.hpp>

namespace peripheral
{
    /**
     * @brief 为外设准备的子线程基类。
     */
    class peripheral_thread
    {
    private:
        /**
         * @brief 管理的线程对象。
         */
        rtos::Thread _thread;
        /**
         * @brief 用于 start 的二元信号量。
         * @note 初始状态为 0，调用 start 后会释放为 1。
         */
        rtos::Semaphore _sem_start{0, 1};
        /**
         * @brief 通用的线程主函数，在其中调用纯虚函数 thread_main。
         * @note
         * 调用纯虚函数时，子类的构造函数必须已经执行完毕，因此应当有一个等待过程。
         * 主函数应当主动调用 start 函数以开始执行 thread_main。
         */
        void _fake_thread_main()
        {
            // 阻塞式地等待主函数请求开始真正的线程。
            _sem_start.acquire();
            // 执行线程主函数。
            thread_main();
        }

    public:
        /**
         * @brief 构造函数将在主线程中调用。
         */
        peripheral_thread()
        {
            // 在构造函数中创建主线程。注意并不立刻执行真正的线程主函数。
            _thread.start(
                std::bind(&peripheral_thread::_fake_thread_main, this));
        }
        /**
         * @brief 析构函数将在主线程中调用。
         *
         * @note 子类被析构时，必须想办法令 thread_main 正常退出。
         * 此析构函数会令线程对象 join。
         *
         * @note 必须先调用 start 函数再析构。
         */
        virtual ~peripheral_thread()
        {
            _thread.join();
            utils::debug_printf("[I] Thread joined.\n");
        }

    protected:
        /**
         * @brief 待实现的纯虚函数，表示线程主函数。
         * 调用 start 函数以真正开始执行。
         *
         * @note 子类被析构时，必须想办法令 thread_main 正常退出。
         */
        virtual void thread_main() = 0;

    public:
        /**
         * @brief 子类的构造函数执行完毕后，在主线程调用此函数以开始执行
         * thread_main。
         * @note 不能在子类的构造函数中调用该函数。
         */
        void start()
        {
            // 令信号量加一，使 _fake_thread_main 跳过阻塞，
            // 以开始执行真正的线程主函数。
            _sem_start.release();
        }
    };
} // namespace peripheral
