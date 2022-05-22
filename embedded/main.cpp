/**
 * @file main.cpp
 * @author UnnamedOrange
 * @brief 主模块与程序入口。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

// 取消注释以进行测试。
// #define RUN_TEST 1
// #define RUN_TEST_ACCEL 1
// #define RUN_TEST_GPS 1

// 取消注释以使用番茄闹钟进行调试。
// #define USE_TFT_FOR_DEBUG_CONSOLE 1

#include "mbed.h"

#include <memory>

#include <peripheral/accel/accel.hpp>
#include <peripheral/bc26/bc26.hpp>
#include <peripheral/buzzer/buzzer.hpp>
#include <peripheral/feedback_message.hpp>
#include <peripheral/feedback_message_queue.hpp>
#include <peripheral/gps/gps.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>
#include <utils/msg_data.hpp>

using namespace std::literals;

class Main
{
    peripheral::feedback_message_queue fmq;
    peripheral::bc26 bc26{fmq};
    std::unique_ptr<peripheral::gps> gps{
        std::make_unique<peripheral::gps>(fmq)};
    peripheral::accel accel{fmq};
    peripheral::buzzer buzzer;

    using sys_clock = Kernel::Clock;
    using pos_t = peripheral::nmea_parser::position_t;

    // 定义状态。

    // 是否是低功耗模式。
    bool low_power_mode = false;
    // 进入低功耗模式倒计时的开始计时时刻。
    sys_clock::time_point count_down_start_time = sys_clock::now();
    // 进入低功耗模式的倒计时预设时间。
    static constexpr auto count_down_elapse = 3min;
    // 上次发送的位置信息。
    pos_t last_pos{};

    /**
     * @brief 系统是否处于低功耗模式。
     */
    bool is_low_power_mode() const
    {
        return low_power_mode;
    }
    /**
     * @brief 进入低功耗模式倒计时是否已结束。
     */
    bool is_count_down_timeout() const
    {
        return sys_clock::now() - count_down_start_time >= count_down_elapse;
    }
    /**
     * @brief 重置倒计时。
     */
    void renew_count_down()
    {
        count_down_start_time = sys_clock::now();
    }
    /**
     * @brief 进入低功耗模式。
     * - 修改状态变量。
     * - 关闭 GPS。
     */
    void invoke_low_power_mode()
    {
        if (!low_power_mode)
        {
            utils::debug_printf("[I] Enter lp.\n");
        }

        low_power_mode = true;
        gps.reset();
        // TODO: 关闭 GPS。
    }
    /**
     * @brief 退出低功耗模式。如果已经退出，只更新状态。
     * - 修改状态变量。
     * - 打开 GPS。
     */
    void revoke_low_power_mode()
    {
        if (low_power_mode)
        {
            utils::debug_printf("[I] Exit lp.\n");
            // TODO: 如果 GPS 的初始化不是什么都不干，需要补充。
            gps = std::move(std::make_unique<peripheral::gps>(fmq));
        }

        low_power_mode = false;
        renew_count_down();
        // TODO: 打开 GPS。

        // 请求获得定位信息。
        gps->request_notify();
    }

    /**
     * @brief 等待初始化完成。
     *
     * @return bool 是否所有模块均已初始化成功。
     */
    bool wait_for_init()
    {
        using fmq_e_t = peripheral::feedback_message_enum_t;
        using utils::msg_data;
        constexpr size_t n_module = 3; // 应当初始化成功的模块数量。
        for (size_t i = 0; i < n_module; i++)
        {
            // 只获取初始化消息。
            // TODO: 评估一直阻塞的可能性。
            auto msg = fmq.get_message(fmq_e_t::init_message_begin,
                                       fmq_e_t::init_message_end);
            switch (msg.first)
            {
            case fmq_e_t::accel_init:
            {
                auto is_success = msg_data<bool>(msg);
                if (is_success)
                {
                    utils::debug_printf("[D] Init accel.\n");
                }
                else
                {
                    utils::debug_printf("[F] Init accel.\n");
                    // 一个模块初始化失败则整体初始化失败。
                    return false;
                }
                break;
            }
            case fmq_e_t::bc26_init:
            {
                // TODO: 记录并使用辅助信息。
                auto [is_success, card_id, is_activated, intensity] =
                    msg_data<std::tuple<bool, std::string, bool, int>>(msg);
                if (is_success)
                {
                    utils::debug_printf("[D] Init bc26.\n");
                }
                else
                {
                    utils::debug_printf("[F] Init bc26.\n");
                    // 一个模块初始化失败则整体初始化失败。
                    return false;
                }
                break;
            }
            case fmq_e_t::gps_init:
            {
                auto is_success = msg_data<bool>(msg);
                if (is_success)
                {
                    utils::debug_printf("[D] Init gps.\n");
                }
                else
                {
                    utils::debug_printf("[F] Init gps.\n");
                    // 一个模块初始化失败则整体初始化失败。
                    return false;
                }
                break;
            }
            default:
            {
                // 未知消息，初始化失败。
                utils::debug_printf("[E] Unknown message.\n");
                return false;
            }
            }
        }
        return true;
    }
    /**
     * @brief 主循环。对应框图右边两列的大循环。
     */
    void main_loop()
    {
        while (true)
        {
            peripheral::feedback_message_queue::message_t msg;
            // 低功耗？
            if (is_low_power_mode())
            {
                // 等待。
                msg = fmq.get_message();
            }
            else
            {
                msg = fmq.peek_message();
            }
            // 数据处理与控制。
            transfer(msg);
        }
    }
    /**
     * @brief 数据处理与控制，即状态转移。
     */
    void transfer(const peripheral::feedback_message_queue::message_t& msg)
    {
        using fmq_e_t = peripheral::feedback_message_enum_t;
        switch (msg.first)
        {
        // 非低功耗模式下，进行额外的处理与控制。
        case fmq_e_t::null:
        {
            on_idle();
            break;
        }
        // 收到加速度计提醒，退出低功耗模式。
        case fmq_e_t::accel_notify:
        {
            on_accel_notify();
            break;
        }
        // 收到新地址，发送并更新状态。
        case fmq_e_t::gps_notify:
        {
            auto pos = utils::msg_data<pos_t>(msg);
            on_gps_notify(pos);
            break;
        }
        // TODO: 实现所有消息的处理。（尤其是 BC26）
        default:
            break;
        }
    }
    /**
     * @brief 非低功耗模式下，轮询检查是否进入低功耗模式。
     *
     * @note 轮询是最简单的，也能保证系统的响应速度最快。
     */
    void on_idle()
    {
        // 此时一定处于非低功耗模式。
        if (is_count_down_timeout())
        {
            // 进入低功耗模式。进入后就一定不会进入 on_idle。
            invoke_low_power_mode();
        }
    }
    void on_accel_notify()
    {
        utils::debug_printf("[I] accel notify.\n");

        // 退出低功耗模式。
        revoke_low_power_mode();
    }
    void on_gps_notify(const pos_t& pos)
    {
        utils::debug_printf("[I] gps notify.\n");

        // TODO: 发送位置。

        // 更新状态。
        last_pos = pos;

        // 如果是非低功耗模式，则请求下一次 GPS 信息。
        if (!is_low_power_mode())
        {
            gps->request_notify();
        }
    }

public:
    Main()
    {
        // 异步初始化各模块。
        {
            utils::debug_printf("[-] Init accel.\n");
            accel.init();
            utils::debug_printf("[-] Init bc26.\n");
            bc26.init();
            utils::debug_printf("[-] Init gps.\n");
            gps->init();
        }

        // 等待初始化完成。
        if (!wait_for_init())
        {
            utils::debug_printf("[E] Init fail.\n");
            return; // 异常情况，退出。
        }

        // 获取位置并发送。
        // 请求等待 GPS 模块发送第一条定位信息。
        gps->request_notify();

        // 消息循环。
        main_loop();
    }
};

// 定义宏以运行测试。
#ifdef RUN_TEST
#include <test/peripheral/accel/test_accel.hpp>
#include <test/peripheral/gps/test_gps.hpp>
#include <test/test_all.hpp>
#endif

int main()
{
    // 定义宏以运行测试。
#ifdef RUN_TEST
    test::test_all();

    // 定义宏以运行测试。该测试是阻塞的。
#ifdef RUN_TEST_ACCEL
    utils::run_app<test::test_accel>();
#endif
    // 定义宏以运行测试。该测试是阻塞的。
#ifdef RUN_TEST_GPS
    utils::run_app<test::test_gps>();
#endif
#endif

    // 主模块发生异常可以直接返回，相当于软件重置。
    while (true)
    {
        utils::run_app<Main>();
        // 正常情况下，主模块不应退出。
        utils::debug_printf("[W] Main exited.\n");
        // 调试时，等待 10 s 再重启。
#if not defined(NDEBUG)
        rtos::ThisThread::sleep_for(10s);
#endif
    }
}
