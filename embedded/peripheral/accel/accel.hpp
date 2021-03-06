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
#include "adxl345_int.hpp"
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
            // 注意死锁。在执行完 release 后一定不能执行 acquire。
            descendant_exit();
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
            descendant_callback_begin();
            switch (static_cast<accel_message_enum_t>(id))
            {
            case accel_message_enum_t::init:
            {
                on_init();
                break;
            }
            case accel_message_enum_t::wait_int:
            {
                on_wait_int();
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
            descendant_callback_end();
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

                // 先复位。复位后没有中断被打开。
                adxl345.software_reset();
                // TODO: 选择合适的数据格式。
                adxl345.set_data_format();
                // 设置 ACTIVITY 阈值。
                // TODO: 使用更好的阈值。
                adxl345.set_threshold_act(10);
                // 设置 ACTIVITY 控制选项。
                // 交流耦合，三轴均有效。
                adxl345.set_act_inact_control(0xF0);
                // 只打开 ACTIVITY 中断，表示监测活动。
                adxl345.set_int_enable(adxl345_int::ACTIVITY);
                // TODO: 选择合适的电源模式。
                adxl345.set_power_control();

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
        void on_wait_int(_fmq_t& fmq)
        {
            // 以下判断在现在仍然是必要的。
            // 有可能释放信号量后，子线程持续运行到下一条消息，
            // 然后主线程才请求父类结束。
            // 加上该判断可以彻底解决死锁问题。
            if (_should_exit) // 如果已经退出，则不获取信号量。
                return; // 防止该子类被销毁后继续使用信号量。

            _sem_irq.acquire(); // 如果没有收到中断，将会一直阻塞。
            if (_should_exit) // 如果已经退出，则不执行后续操作。
                return;

            // 参见 feedback_message_enum_t::accel_notify。
            fmq.post_message_unique(_fmq_e_t::accel_notify, nullptr);

            // 休眠以防止中断触发过于频繁。
            using namespace std::literals;
            rtos::ThisThread::sleep_for(1000ms);
            // 读取中断源以清除中断标志。
            adxl345.get_int_source(); // 结果不使用，因为只用一个中断。
        }
        void on_wait_int()
        {
            on_wait_int(_external_fmq);
        }

        // 以下函数是主模块的接口，均在主线程中运行。
    public:
        /**
         * @brief 初始化。
         */
        void init()
        {
            post_message(static_cast<int>(accel_message_enum_t::init), nullptr);
        }
        /**
         * @brief 等待中断。如果没有收到中断，将会一直阻塞。
         *
         * @note 该函数会在消息队列为空时自动被调用。
         */
        void wait_int()
        {
            post_message_unique(
                static_cast<int>(accel_message_enum_t::wait_int), nullptr);
        }
    };
} // namespace peripheral
