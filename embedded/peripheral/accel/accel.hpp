/**
 * @file accel.hpp
 * @author UnnamedOrange
 * @brief 加速度计模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include "../command_spi.hpp"
#include "../feedback_message.hpp"
#include "../feedback_message_queue.hpp"
#include "../global_peripheral.hpp"
#include "../peripheral_std_framework.hpp"
#include "accel_message.hpp"
#include "adxl345_middleware.hpp"
#include <utils/debug.hpp>

namespace peripheral
{
    class accel : public peripheral_std_framework
    {
    private:
        using _fmq_t = feedback_message_queue;
        using _fmq_e_t = feedback_message_enum_t;

    protected:
        adxl345_middleware adxl345;
        _fmq_t& _external_fmq;

    public:
        accel(_fmq_t& fmq) : _external_fmq(fmq)
        {
        }

        // 以下函数是子模块的回调函数，均在子线程中运行。
    private:
        void on_message(int id, std::shared_ptr<void> data) override
        {
            switch (static_cast<accel_message_enum_t>(id))
            {
            case accel_message_enum_t::init:
            {
                on_init();
                break;
            }
            default:
            {
                break;
            }
            }
        }
        /**
         * @brief 初始化。
         */
        void on_init(_fmq_t& fmq)
        {
            bool is_success = false;
            do
            {
                if (!adxl345.check_devid()) // 初始化失败，
                    break;                  // 检查接线是否正确。

                adxl345.software_reset(); // 先复位。复位后没有中断被打开。
                adxl345.set_data_format(); // TODO: 选择合适的数据格式。
                // TODO: 设置中断回调函数。

                adxl345.set_int_enable(); // TODO: 选择合适的中断源。
                adxl345.set_power_control(); // TODO: 选择合适的电源模式。

                if (!adxl345.check_devid()) // 初始化失败，
                    break;                  // 检查程序是否正确。

                is_success = true;
            } while (false);

            // 参见 feedback_message_enum_t::accel_init。
            fmq.post_message(_fmq_e_t::accel_init,
                             std::make_shared<bool>(is_success));
        }
        void on_init()
        {
            on_init(_external_fmq);
        }

        // 以下函数是主模块的接口，均在主线程中运行。
    public:
        /**
         * @brief 初始化。
         */
        void init()
        {
            push(static_cast<int>(accel_message_enum_t::init), nullptr);
        }
    };
} // namespace peripheral
