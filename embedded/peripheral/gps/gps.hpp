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
        command_sender_serial sender{serial_gps};
        command_receiver_serial receiver{serial_gps};
        _fmq_t& _external_fmq;

    private:
        nmea_parser parser{receiver};

    public:
        gps(_fmq_t& fmq) : _external_fmq(fmq)
        {
        }

        /**
         * @brief 以下函数是子模块的回调函数，均在子线程中运行。
         */
    private:
        void on_message(int id, std::shared_ptr<void> data) override
        {
            switch (static_cast<gps_message_enum_t>(id))
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
