/**
 * @file test_peripheral_std_framework.hpp
 * @author UnnamedOrange
 * @brief 测试 peripheral/peripheral_std_framework.hpp。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <peripheral/peripheral_std_framework.hpp>
#include <utils/app.hpp>
#include <utils/debug.hpp>

namespace test
{
    /**
     * @brief 测试 peripheral_std_framework。
     * - 测试能否正常收发消息。
     * - 测试能否在子线程内向自己 push 消息。（是可以的）
     */
    class test_peripheral_std_framework
    {
        struct _test_struct
        {
            std::string str;
            std::vector<int> vec;
        };
        class _fake_peripheral : public peripheral::peripheral_std_framework
        {
            void on_message(int id, std::shared_ptr<void> data) override
            {
                switch (id)
                {
                case 0:
                {
                    utils::debug_printf("[I] OK. Message id 0 received.\n");
                    break;
                }
                case 1:
                {
                    int value = *std::static_pointer_cast<int>(data);
                    utils::debug_printf(
                        "[I] OK. Message id 1 received with parameter %d.\n",
                        value);
                    break;
                }
                case 2:
                {
                    const _test_struct& value =
                        *std::static_pointer_cast<_test_struct>(data);
                    std::string vec_str;
                    for (const auto& t : value.vec)
                        vec_str += std::to_string(t) + " ";
                    utils::debug_printf(
                        "[I] OK. Message id 2 received with parameter "
                        "\"%s\" and vector %s.\n",
                        value.str.c_str(), vec_str.c_str());
                    break;
                }
                case 3:
                {
                    utils::debug_printf(
                        "[I] Message id 3 received. 0 is pushed.\n");
                    push(0, nullptr);
                    break;
                }
                }
            }
        } _fp;

    public:
        test_peripheral_std_framework()
        {
            utils::debug_printf("\n");
            utils::debug_printf(
                "[I] Test for peripheral_std_framework starts 1 second "
                "later.\n");
            rtos::ThisThread::sleep_for(1s);
            _fp.start();

            // 测试无参数消息的收发。
            _fp.push(0, nullptr);
            // 测试简单参数消息的收发。
            _fp.push(1, std::make_shared<int>(0));
            // 测试复杂参数消息的收发。
            _fp.push(2, std::make_shared<_test_struct>(_test_struct{
                            "OK.", std::vector<int>{114514, 1919, 810}}));

            // 测试能否在子线程内向自己 push 消息。
            _fp.push(3, nullptr);

            // 等待子线程输出结束后再输出提示信息。
            rtos::ThisThread::sleep_for(1s);
            // 收到警告是正常现象。
            utils::debug_printf(
                "[I] The warning 1 second later can be ignored.\n");
            // 之后的延迟来源于 peripheral_std_framework 的析构函数。
        }
    };
} // namespace test