/**
 * @file buzzer.hpp
 * @author UnnamedOrange
 * @brief 蜂鸣器模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>

#include "../global_peripheral.hpp"
#include "../peripheral_std_framework.hpp"
#include "buzzer_message.hpp"

namespace peripheral
{
    class buzzer : public peripheral_std_framework
    {
    private:
        // TODO: 确定是高使能还是低使能。
        constexpr static int EN_ON = 1; // 使用 !EN_ON 表示不使能。
        constexpr static int period_us = 1'000'000 / 440; // 440 Hz。

    protected:
        mbed::DigitalOut _buzzer_en{PIN_BUZZER_EN};
        mbed::PwmOut _buzzer_out{PIN_BUZZER};

    private:
        bool is_buzzing{};

    public:
        buzzer()
        {
            _buzzer_en = !EN_ON;
            _buzzer_out.period_us(period_us);
        }
        ~buzzer()
        {
            descendant_exit();
        }

        // 以下函数是子模块的回调函数，均在子线程中运行。
    private:
        void on_message(int id, std::shared_ptr<void> data) override
        {
            descendant_callback_begin();
            switch (static_cast<buzzer_message_enum_t>(id))
            {
            case buzzer_message_enum_t::buzz:
            {
                on_buzz();
                break;
            }
            default:
            {
                break;
            }
            }
            descendant_callback_end();
        }
        /**
         * @brief 蜂鸣器响一会儿。
         */
        void on_buzz()
        {
            is_buzzing = true;
            do
            {
                using namespace std::literals;
                _buzzer_en = EN_ON;
                _buzzer_out.write(0.5f);
                rtos::ThisThread::sleep_for(1s);
            } while (false);
            _buzzer_out.write(0.0f);
            _buzzer_en = !EN_ON;
            is_buzzing = false;
        }

        // 以下函数是主模块的接口，均在主线程中运行。
    public:
        /**
         * @brief 蜂鸣器响一会儿。
         */
        void buzz()
        {
            if (!is_buzzing) // 完全忽略所有的重复请求。
            {
                post_message_unique(
                    static_cast<int>(buzzer_message_enum_t::buzz), nullptr);
            }
        }
    };
} // namespace peripheral
