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

#include "mbed.h"

#include <peripheral/accel/accel.hpp>
#include <peripheral/bc26/bc26.hpp>
#include <peripheral/feedback_message.hpp>
#include <peripheral/feedback_message_queue.hpp>
#include <peripheral/gps/gps.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>

using namespace std::literals;

class Main
{
    peripheral::feedback_message_queue fmq;
    peripheral::bc26 bc26{fmq};
    peripheral::gps gps{fmq};
    peripheral::accel accel{fmq};

public:
    Main()
    {
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
