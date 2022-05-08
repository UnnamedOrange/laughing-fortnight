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

namespace peripheral
{
    class accel : public peripheral_std_framework
    {
    private:
        using _fmq_t = feedback_message_queue;
        using _fmq_e_t = feedback_message_enum_t;

    private:
        mbed::SPI spi_accel{PIN_ACCEL_MOSI, PIN_ACCEL_MISO, PIN_ACCEL_SCLK};
        mbed::DigitalOut cs_accel{PIN_ACCEL_CS};

    protected:
        command_spi<> sender{spi_accel, cs_accel}; // 使用默认模板参数。
        _fmq_t& _external_fmq;

    public:
        accel(_fmq_t& fmq) : _external_fmq(fmq)
        {
        }

        /**
         * @brief 以下函数是子模块的回调函数，均在子线程中运行。
         */
    private:
        void on_message(int id, std::shared_ptr<void> data) override
        {
            switch (static_cast<accel_message_enum_t>(id))
            {
            default:
            {
                break;
            }
            }
        }

        /**
         * @brief 以下函数是主模块的接口，均在主线程中运行。
         */
    public:
    };
} // namespace peripheral
