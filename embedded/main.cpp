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

// 取消注释以使用番茄闹钟进行调试。
// #define USE_TFT_FOR_DEBUG_CONSOLE 1

#include "mbed.h"

#include <memory>

#include <peripheral/accel/accel.hpp>
#include <peripheral/bc26/bc26.hpp>
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
    peripheral::gps gps{fmq};
    peripheral::accel accel{fmq};

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
            gps.init();
        }

        // 等待初始化完成。
        if (!wait_for_init())
        {
            utils::debug_printf("[E] Init fail.\n");
            return; // 异常情况，退出。
        }
    }
};

// 定义宏以运行测试。
#ifdef RUN_TEST
#include <test/peripheral/accel/test_accel.hpp>
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
