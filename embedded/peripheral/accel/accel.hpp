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
            adxl345.set_int1(std::bind(&accel::irq_callback, this));
        }
        ~accel()
        {
            adxl345.reset_int1(); // 防止在信号量销毁后收到中断请求。
            _should_exit = true;
            _sem_irq.release(); // 强制释放信号量，以正常退出。
        }

    private:
        bool _should_exit{};
        rtos::Semaphore _sem_irq{0, 1};
        void irq_callback()
        {
            _sem_irq.try_acquire(); // 先获取再释放，保证接下来可释放。
            _sem_irq.release(); // 释放信号量，可获取的信号量就表示有事件。
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
            case accel_message_enum_t::wait_int:
            {
                on_wait();
                break;
            }
            default:
            {
                break;
            }
            }
            if (empty()) // 如果消息队列已空，自动等待下一次中断。
            {
                wait_int();
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

        /**
         * @brief 等待中断。如果没有收到中断，将会一直阻塞。
         */
        void on_wait(_fmq_t& fmq)
        {
            _sem_irq.acquire(); // 如果没有收到中断，将会一直阻塞。
            if (_should_exit) // 如果已经退出，则不执行，
                return; // 并且之后不会有新消息，所以前面无需再判断。

            // 参见 feedback_message_enum_t::accel_notify。
            fmq.post_message(_fmq_e_t::accel_notify, nullptr);
        }
        void on_wait()
        {
            on_wait(_external_fmq);
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
        /**
         * @brief 等待中断。如果没有收到中断，将会一直阻塞。
         */
        void wait_int()
        {
            push(static_cast<int>(accel_message_enum_t::wait_int), nullptr);
        }
    };
} // namespace peripheral
