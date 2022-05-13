/**
 * @file feedback_message.hpp
 * @author UnnamedOrange
 * @brief 定义子模块向主模块的反馈消息。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

namespace peripheral
{
    enum class feedback_message_enum_t : int
    {
        /**
         * @brief 没有消息。用于消息队列为空的情况。
         */
        null,
        _message_begin = null,

        /**
         * @brief 各模块初始化消息的起始点。
         */
        init_message_begin,
        /**
         * @brief 加速度计初始化的反馈信息。
         *
         * @param bool 是否初始化成功。
         */
        accel_init,
        /**
         * @brief BC26 模块综合初始化的反馈信息。
         *
         * @param bool 是否初始化成功。
         * @param string 卡号。
         * @param bool 是否已激活。
         * @param int 信号强度。
         */
        bc26_init,
        /**
         * @brief GPS 模块初始化的反馈信息。
         *
         * @param bool 是否初始化成功。
         */
        gps_init,
        /**
         * @brief 各模块初始化消息的终止点。
         */
        init_message_end,

        /**
         * @brief 加速度计模块消息的起始点。不包含初始化消息。
         */
        accel_message_begin,
        /**
         * @brief 加速度计被移动的通知。
         */
        accel_notify,
        /**
         * @brief 加速度计模块消息的终止点。不包含初始化消息。
         */
        accel_message_end,

        /**
         * @brief BC26 模块消息的起始点。不包含初始化消息。
         */
        bc26_message_begin,
        /**
         * @brief BC26 模块 send_at 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         */
        bc26_send_at,
        /**
         * @brief BC26 模块 software_reset 的反馈信息。
         */
        bc26_software_reset,
        /**
         * @brief BC26 模块 send_ate 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         */
        bc26_send_ate,
        /**
         * @brief BC26 模块 send_at_cfun_set 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         */
        bc26_send_at_cfun_set,
        /**
         * @brief BC26 模块 send_at_cimi 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         * @param string 卡号。
         */
        bc26_send_at_cimi,
        /**
         * @brief BC26 模块 send_at_cgatt_get 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         * @param bool 是否已激活。
         */
        bc26_send_at_cgatt_get,
        /**
         * @brief BC26 模块 send_at_cesq 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         * @param int 信号强度。
         */
        bc26_send_at_cesq,
        /**
         * @brief BC26 模块消息的终止点。不包含初始化消息。
         */
        bc26_message_end,

        /**
         * @brief GPS 模块消息的起始点。不包含初始化消息。
         */
        gps_message_begin,
        /**
         * @brief GPS 模块有新位置的通知。
         *
         * @param nmea_parser::position_t 位置信息。
         */
        gps_notify,
        /**
         * @brief GPS 模块消息的终止点。不包含初始化消息。
         */
        gps_message_end,

        _message_end,
    };
} // namespace peripheral
