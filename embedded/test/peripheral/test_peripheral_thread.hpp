/**
 * @file test_peripheral_thread.hpp
 * @author UnnamedOrange
 * @brief 测试 peripheral/peripheral_thread.hpp。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <chrono>

#include <peripheral/peripheral_thread.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>

namespace test
{
    /**
     * @brief 测试 peripheral_thread。
     * - 测试子线程是否能在主线程中受控延迟启动。
     * - 测试子模块被销毁时，是否正常 join。
     */
    class test_peripheral_thread
    {
        class _fake_peripheral : public peripheral::peripheral_thread
        {
            void thread_main() override
            {
                utils::debug_printf("[I] thread_main starts.\n");
                // 子线程立即结束。
            }

        public:
            ~_fake_peripheral()
            {
                peripheral_thread::join();
            }
        };
        _fake_peripheral _fp;

    public:
        test_peripheral_thread()
        {
            using namespace std::literals;
            utils::debug_printf("\n");
            utils::debug_printf("[I] peripheral_thread test.\n");
            utils::debug_printf("[I] OK if info occurs.\n");
            // 延迟一秒，期望一秒后看到子线程开始的信息。
            rtos::ThisThread::sleep_for(1s);
            _fp.start();
            // 不是死循环，主模块立即被销毁，期望看到子线程 join。
        }
    };
} // namespace test