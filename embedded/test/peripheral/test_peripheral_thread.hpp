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
     * - 期望主线程是一个死循环，主模块及其里面的子模块永不被销毁。
     *   测试在子模块被销毁时，是否正常报错。
     */
    class test_peripheral_thread
    {
        class _fake_peripheral : public peripheral::peripheral_thread
        {
            void thread_main() override
            {
                // 子线程立即结束。
            }
        };
        _fake_peripheral _fp;

    public:
        test_peripheral_thread()
        {
            debug("You should see a warning 5 seconds later.\n");
            rtos::ThisThread::sleep_for(5s);
            _fp.start();
            // 不是死循环，主模块立即被销毁，期望看到报错。
        }
    };
} // namespace test