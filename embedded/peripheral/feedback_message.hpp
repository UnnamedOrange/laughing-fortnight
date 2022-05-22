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
         * @brief BC26 模块 send_at_qmtcfg 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         */
        bc26_send_at_qmtcfg,
        /**
         * @brief BC26 模块 send_at_qmtopen 的反馈消息。
         *
         * @param bool 是否成功收到 OK 且成功解析结果。
         * @param int MQTT Socket 标识符。范围 0-5。
         * @param int 命令执行结果。
         * - -1 打开网络失败。
         * - 0 打开网络成功。
         * - 1 参数错误。
         * - 2 MQTT 标识符被占用。
         * - 3 激活 PDP 失败。
         * - 4 域名解析失败。
         * - 5 网络断开错误。
         */
        bc26_send_at_qmtopen,
        /**
         * @brief BC26 模块 send_at_qmtclose 的反馈消息。
         *
         * @param bool 是否成功收到 OK 且成功解析结果。
         * @param int MQTT Socket 标识符。范围 0-5。
         * @param int 命令执行结果。
         * - -1 关闭网络失败。
         * - 0 关闭网络成功。
         */
        bc26_send_at_qmtclose,
        /**
         * @brief BC26 模块 send_at_qmtconn 的反馈消息。
         *
         * @param bool 是否成功收到 OK 且成功解析结果。
         * @param int MQTT Socket 标识符。范围 0-5。
         * @param int 命令执行结果。
         * - 0 数据包发送成功且接收到服务器的 ACK。
         * - 1 数据包重传。
         * - 2 数据包发送失败。
         * @param int 连接返回码。
         * - 0 接受连接。
         * - 1 拒绝连接：不接受的协议版本。
         * - 2 拒绝连接：标识符被拒绝。
         * - 3 拒绝连接：服务器不可用。
         * - 4 拒绝连接：错误的用户名或密码。
         * - 5 拒绝连接：未授权。
         */
        bc26_send_at_qmtconn,
        /**
         * @brief BC26 模块 send_at_qmtdisc 的反馈消息。
         *
         * @param bool 是否成功收到 OK 且成功解析结果。
         * @param int MQTT Socket 标识符。范围 0-5。
         * @param int 命令执行结果。
         * - -1 断开连接失败。
         * - 0 断开连接成功。
         */
        bc26_send_at_qmtdisc,
        /**
         * @brief BC26 模块 send_at_qmtsub 的反馈信息。
         *
         * @param bool 是否成功收到 OK 且成功解析结果。
         * @param int MQTT Socket 标识符。范围 0-5。
         * @param int 数据包标识符，范围：1-65535。
         * @param int 命令执行结果。
         * - 0 数据包发送成功且接收到服务器的 ACK。
         * - 1 数据包重传。
         * - 2 数据包发送失败。
         * @param int
         * - 若命令执行结果为 0，则作为已确认 QoS 等级的矢量。同时，该参数值为
         * 128，表示服务器拒绝订阅。
         * - 若命令执行结果为 1，则表示数据包重传次数。
         * - 若命令执行结果为 2，则不显示。
         */
        bc26_send_at_qmtsub,
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
