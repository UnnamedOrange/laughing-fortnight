/**
 * @file test_buzzer.hpp
 * @author UnnamedOrange
 * @brief 测试蜂鸣器模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>

#include <peripheral/buzzer/buzzer.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>

namespace test
{
    /**
     * @brief 测试蜂鸣器模块。
     */
    class test_buzzer
    {
        peripheral::buzzer buzzer{};

    public:
        test_buzzer()
        {
            using namespace std::literals;
            utils::debug_printf("\n");
            utils::debug_printf("[I] buzzer test.\n");

            // 测试一般地响一次。
            utils::debug_printf("[-] once\n");
            {
                buzzer.buzz();
                rtos::ThisThread::sleep_for(1s);
            }
            utils::debug_printf("[D] once\n");
            rtos::ThisThread::sleep_for(1s);

            // 测试能否忽略重复请求。
            utils::debug_printf("[-] ignore rep\n");
            {
                // 期望响 2 次，间隔 200 ms。
                buzzer.buzz();
                buzzer.buzz();
                rtos::ThisThread::sleep_for(1s);
                rtos::ThisThread::sleep_for(200ms);
                buzzer.buzz();
                rtos::ThisThread::sleep_for(1s);
            }
            utils::debug_printf("[D] ignore rep\n");
            rtos::ThisThread::sleep_for(1s);
        }
    };
} // namespace test
