/**
 * @file gps.hpp
 * @author UnnamedOrange
 * @brief GPS 模块。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <chrono>
#include <string>
#include <tuple>

#include "../command_receiver_serial.hpp"
#include "../command_sender_serial.hpp"
#include "../feedback_message.hpp"
#include "../feedback_message_queue.hpp"
#include "../global_peripheral.hpp"
#include "../peripheral_std_framework.hpp"
#include "gps_message.hpp"
#include "nmea_parser.hpp"

namespace peripheral
{
    class gps : public peripheral_std_framework
    {
    private:
        using _fmq_t = feedback_message_queue;
        using _fmq_e_t = feedback_message_enum_t;

    protected:
        mbed::BufferedSerial serial_gps{PIN_GPS_TX, PIN_GPS_RX};
        command_sender_serial sender{serial_gps};
        command_receiver_serial receiver{serial_gps};
        _fmq_t& _external_fmq;

    private:
        nmea_parser parser{receiver};

    private:
        bool should_exit{};

    public:
        gps(_fmq_t& fmq) : _external_fmq(fmq)
        {
        }
        ~gps()
        {
            should_exit = true;
            descendant_exit();
        }

        // 以下函数是子模块的回调函数，均在子线程中运行。
    private:
        void on_message(int id, std::shared_ptr<void> data) override
        {
            descendant_callback_begin();
            switch (static_cast<gps_message_enum_t>(id))
            {
            case gps_message_enum_t::init:
            {
                on_init();
                break;
            }
            case gps_message_enum_t::request_notify:
            {
                on_request_notify();
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
         * @brief 初始化。
         */
        void on_init(_fmq_t& fmq)
        {
            // TODO: 补充 GPS 初始化的流程。
            bool is_success = true;

            // 参见 feedback_message_enum_t::gps_init。
            fmq.post_message(_fmq_e_t::gps_init,
                             std::make_shared<bool>(is_success));
        }
        void on_init()
        {
            on_init(_external_fmq);
        }
        /**
         * @brief 请求在位置信息更新时通知外部队列。
         *
         * @note 该消息会阻塞队列。
         *
         * @todo 尽可能避免该消息阻塞队列。
         */
        void on_request_notify(_fmq_t& fmq)
        {
            auto previous = parser.get_last_valid_position();
            while (true)
            {
                using namespace std::literals;

                // 检查是否应该退出，避免阻塞析构函数。
                if (should_exit)
                    break;

                // 等待内部刷新。
                rtos::ThisThread::sleep_for(1s);
                auto current = parser.get_last_valid_position();
                // 判断这两个对象不同用一个较弱的条件即可。
                if (current.is_valid && (current.second != previous.second ||
                                         current.minute != previous.minute))
                {
                    // 参见 feedback_message_enum_t::gps_notify。
                    fmq.post_message(
                        _fmq_e_t::gps_notify,
                        std::make_shared<nmea_parser::position_t>(current));
                    break;
                }
            }
        }
        void on_request_notify()
        {
            on_request_notify(_external_fmq);
        }

        // 以下函数是主模块的接口，均在主线程中运行。
    public:
        /**
         * @brief 初始化。
         */
        void init()
        {
            push(static_cast<int>(gps_message_enum_t::init), nullptr);
        }
        /**
         * @brief 请求在位置信息更新时通知外部队列。
         */
        void request_notify()
        {
            if (!count(static_cast<int>(gps_message_enum_t::request_notify)))
            {
                push(static_cast<int>(gps_message_enum_t::request_notify),
                     nullptr);
            }
        }

        /**
         * @brief 获取当前的位置信息。
         *
         * @note 该函数是线程安全的。
         *
         * @note 该函数不涉及 GPS 模块的消息队列。
         *
         * @return nmea_parser::position_t
         */
        nmea_parser::position_t get_current_position()
        {
            return parser.get_current_position();
        }
        /**
         * @brief 获取最后一次有效的位置信息。
         *
         * @note 该函数是线程安全的。
         *
         * @note 该函数不涉及 GPS 模块的消息队列。
         *
         * @return nmea_parser::position_t
         */
        nmea_parser::position_t get_last_valid_position()
        {
            return parser.get_last_valid_position();
        }
    };
} // namespace peripheral
