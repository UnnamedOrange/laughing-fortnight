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

namespace test
{
    /**
     * @brief 测试 peripheral_thread。
     * - 测试子线程是否能在主线程中受控延迟启动。
     * - 期望主线程是一个死循环，主模块及其里面的子模块永不被销毁。
     *   测试在子模块被销毁时，是否正常报错。
     */
    class test_peripheral_thread
    {
        class _fake_peripheral : public peripheral::peripheral_thread
        {
            void thread_main() override
            {
                debug("[Info] thread_main starts.\n");
                // 子线程立即结束。
            }
        };
        _fake_peripheral _fp;

    public:
        test_peripheral_thread()
        {
            debug("\n");
            debug("[Info] Test for peripheral_thread starts 1 second later.\n");
            debug("[Info] An info and a warning expected 5 seconds later.\n");
            // 延迟五秒，期望五秒后看到子线程开始的信息。
            rtos::ThisThread::sleep_for(5s);
            _fp.start();
            // 不是死循环，主模块立即被销毁，期望看到报错。
        }
    };
} // namespace test