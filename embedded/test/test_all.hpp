/**
 * @file test_all.hpp
 * @author UnnamedOrange
 * @brief 执行所有测试。要执行的测试需要在该文件中手动添加。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <utils/app.hpp>

#include "peripheral/buzzer/test_buzzer.hpp"
#include "peripheral/test_feedback_message_queue.hpp"
#include "peripheral/test_peripheral_std_framework.hpp"
#include "peripheral/test_peripheral_thread.hpp"

namespace test
{
    /**
     * @brief 执行所有测试。需在此处手动添加要测试的 app 类。
     *
     * @note 要执行所有测试，测试类不应抛出异常。
     *
     * @note 在 main 函数中调用该函数。
     */
    inline void test_all()
    {
        // 在此处添加要测试的 app 类。
        utils::run_app<test_buzzer>();
        utils::run_app<test_feedback_message_queue>();
        utils::run_app<test_peripheral_thread>();
        utils::run_app<test_peripheral_std_framework>();
    }
} // namespace test
