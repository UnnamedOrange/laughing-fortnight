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
// #define RUN_TEST_BC26 1
// #define RUN_TEST_GPS 1

// 取消注释以使用番茄闹钟进行调试。
// #define USE_TFT_FOR_DEBUG_CONSOLE 1

#include "mbed.h"

#include <memory>

#include <peripheral/accel/accel.hpp>
#include <peripheral/bc26/bc26.hpp>
#include <peripheral/bc26/bc26_config.hpp>
#include <peripheral/buzzer/buzzer.hpp>
#include <peripheral/feedback_message.hpp>
#include <peripheral/feedback_message_queue.hpp>
#include <peripheral/global_peripheral.hpp>
#include <peripheral/gps/gps.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>
#include <utils/msg_data.hpp>

using namespace std::literals;

class Main
{
    peripheral::feedback_message_queue fmq;
    peripheral::bc26 bc26{fmq};
    mbed::DigitalOut gps_en{PIN_GPS_EN};
    std::unique_ptr<peripheral::gps> gps{
        std::make_unique<peripheral::gps>(fmq)};
    peripheral::accel accel{fmq};
    peripheral::buzzer buzzer;

    using sys_clock = Kernel::Clock;
    using pos_t = peripheral::nmea_parser::position_t;

    /**
     * @brief 生成要发送的位置信息字符串。
     * 格式：pos: 纬度,经度;
     *
     * @param pos 位置信息。
     * @return std::string 可发送的字符串。
     */
    static std::string make_sent_string(const pos_t& pos)
    {
        if (!pos.is_valid)
            return "";
        std::string ret = "pos: ";
        ret += pos.latitude;
        ret += "," + pos.longitude;
        ret += ";";
        return ret;
    }

    // 定义状态。

    // 是否是低功耗模式。
    bool low_power_mode = false;
    // 进入低功耗模式倒计时的开始计时时刻。
    sys_clock::time_point count_down_start_time = sys_clock::now();
    // 进入低功耗模式的倒计时预设时间。
    static constexpr auto count_down_elapse = 3min;
    // 是否已经连接服务器。
    bool is_server_connected = false;
    // 上次发送的位置信息。
    pos_t last_pos{};
    // 上次收到心跳的时刻。
    sys_clock::time_point last_pulse_time = sys_clock::now();
    // 心跳维持的预设时间。
    static constexpr auto pulse_time_elapse = 2min;

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
        // 关闭 GPS。
        gps_en = 1;
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
        // 打开 GPS。
        gps_en = 0;

        // 请求获得定位信息。
        gps->request_notify();
    }

    // 尝试连接服务器的次数。尝试次数太多直接重置整个系统。
    int try_connect_times{};
    /**
     * @brief 异步请求连接服务器。
     */
    void connect_server()
    {
        try_connect_times++;
        if (try_connect_times > 10)
        {
            fmq.post_message(peripheral::feedback_message_enum_t::quit,
                             nullptr);
        }
        else
        {
            bc26.send_at_qiclose();
            bc26.send_at_qiopen(remote_address, remote_port);
        }
    }
    /**
     * @brief 检查是否需要发送数据。如果需要，就发送 last_pos。
     */
    void check_and_send_position()
    {
        auto content = make_sent_string(last_pos);
        if (is_server_connected) // 如果服务器已连接则发送，否则不发送。
        {
            bc26.send_at_qisend(content);
        }
    }
    /**
     * @brief 根据远程发送的指令进行操作。
     *
     * @param command 远程发送的指令。
     */
    void check_command(const std::string& command)
    {
        if (false) // 方便对齐下面的代码。
        {
        }
        else if (command == "buzz") // 蜂鸣器响的指令。
        {
            utils::debug_printf("[I] buzz.\n");
            // 让蜂鸣器响。
            buzzer.buzz();
        }
        else if (command == "pulse") // 心跳。
        {
            utils::debug_printf("[I] pulse.\n");
            // 更新心跳时间。
            last_pulse_time = sys_clock::now();
        }
        else
        {
            utils::debug_printf("[W] Unknown message\n");
        }
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
            if (std::get<0>(msg) == peripheral::feedback_message_enum_t::quit)
                break;
            else
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
        case fmq_e_t::bc26_send_at_qiopen:
        {
            using param_type = std::tuple<bool, int, int>;
            const auto& t = utils::msg_data<param_type>(msg);
            on_bc26_send_at_qiopen(std::get<0>(t), std::get<1>(t),
                                   std::get<2>(t));
            break;
        }
        case fmq_e_t::bc26_send_at_qiclose:
        {
            auto is_ok = utils::msg_data<bool>(msg);
            on_bc26_send_at_qiclose(is_ok);
            break;
        }
        case fmq_e_t::bc26_send_at_qisend:
        {
            auto is_ok = utils::msg_data<bool>(msg);
            on_bc26_send_at_qisend(is_ok);
            break;
        }
        case fmq_e_t::bc26_send_at_qird:
        {
            using param_type = std::tuple<bool, std::string>;
            const auto& t = utils::msg_data<param_type>(msg);
            on_bc26_send_at_qird(std::get<0>(t), std::get<1>(t));
            break;
        }
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

        // 更新状态。
        last_pos = pos;

        // 发送位置。
        check_and_send_position();

        // 如果是非低功耗模式，则请求下一次 GPS 信息。
        if (!is_low_power_mode())
        {
            gps->request_notify();
        }
    }
    void on_bc26_send_at_qiopen(bool is_ok, int connect_id, int result)
    {
        if (is_ok && !result) // 如果服务器连接成功。
        {
            is_server_connected = true; // 更新状态。
            bc26.send_at_qird();        // 开始轮询。
        }
        else
        {
            is_server_connected = false; // 保证状态变量正确。
            // 等待 5 s 后，尝试重新连接服务器。
            rtos::ThisThread::sleep_for(5s);
            connect_server();
        }
    }
    void on_bc26_send_at_qiclose(bool is_ok)
    {
        is_server_connected = false; // 更新状态。
        // 如果失败，就尝试重连。在 qiopen 中处理。
    }
    void on_bc26_send_at_qisend(bool is_ok)
    {
        // 如果失败，认为服务器已断开连接。
        if (!is_ok)
        {
            is_server_connected = false;
            connect_server(); // 异步请求重新连接服务器。
        }
    }
    void on_bc26_send_at_qird(bool is_ok, const std::string& content)
    {
        // 如果失败，认为服务器已断开连接。
        if (!is_ok)
        {
            is_server_connected = false;
            connect_server(); // 异步请求重新连接服务器。
            return;
        }
        // 否则，根据内容转移状态，并且等待 1 s 轮询。
        if (content.length())
            check_command(content);
        // 如果没有收到心跳，则认为已断开连接。
        if (sys_clock::now() - last_pulse_time > pulse_time_elapse)
        {
            utils::debug_printf("[W] pulse reset\n");
            is_server_connected = false;
            connect_server(); // 异步请求重新连接服务器。
        }
        else if (is_server_connected)
        {
            // 等待 1 s 轮询。
            rtos::ThisThread::sleep_for(1s);
            bc26.send_at_qird();
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

        // 连接服务器。
        connect_server();

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
#include <test/peripheral/bc26/test_bc26.hpp>
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
#ifdef RUN_TEST_BC26
    utils::run_app<test::test_bc26>();
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
