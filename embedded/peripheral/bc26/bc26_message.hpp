/**
 * @file bc26_message.hpp
 * @author UnnamedOrange
 * @brief 定义 BC26 的消息。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

namespace peripheral
{
    enum class bc26_message_t : int
    {
        _message_begin,

        /**
         * @brief 重复发送 AT 指令。
         *
         * @param int 最大重试次数。
         */
        send_at,
        /**
         * @brief 发送 AT+QRST=1 指令。软件重置。
         */
        software_reset,
        /**
         * @brief 发送 ATE<echo> 指令。打开或关闭回显。
         *
         * @param bool 是否打开回显。
         */
        send_ate,
        /**
         * @brief 发送 AT+CFUN=<mode> 指令。设置功能模式。
         *
         * @param int 功能模式。
         */
        send_at_cfun_set,
        /**
         * @brief 发送 AT+CIMI 指令。查询卡号。
         */
        send_at_cimi,
        /**
         * @brief 发送 AT+CGATT? 指令。查询激活状态。
         */
        send_at_cgatt_get,
        /**
         * @brief 发送 AT+CESQ 指令。获取信号质量。
         */
        send_at_cesq,

        /**
         * @brief 综合地初始化。
         *
         * @param int 最大重试次数。
         */
        init,

        /**
         * @brief 发送 AT+QMTCFG= 指令。配置 MQTT 可选参数。
         *
         * @param std::string 类型。不包含引号。例如 dataformat。
         * @param std::vector<std::string>
         * 参数列表。各参数将会被逗号隔开，需要手动添加引号。
         */
        send_at_qmtcfg,
        /**
         * @brief 发送 AT+QMTOPEN= 指令。打开 MQTT 客户端网络。
         *
         * @param int MQTT Socket 标识符。范围 0-5。
         * @param std::string 服务器地址，可以是 IP 地址或者域名。最大长度 100
         * 字节。不包含引号。
         * @param int 服务器端口。范围 1-65535。
         */
        send_at_qmtopen,
        /**
         * @brief 发送 AT+QMTCLOSE= 指令。关闭 MQTT 客户端网络。
         *
         * @param int MQTT Socket 标识符。范围 0-5。
         */
        send_at_qmtclose,
        /**
         * @brief 发送 AT+QMTCONN= 指令。客户端连接 MQTT 服务器。
         *
         * @param int MQTT Socket 标识符。范围 0-5。
         * @param std::string 客户端标识符。不包含引号。
         * @param std::string 客户端用户名，可用来鉴权。不包含引号。
         * @param std::string 客户端用户名对应的密码，可用来鉴权。不包含引号。
         */
        send_at_qmtconn,

        _message_end,
        /**
         * @brief 被定义的消息的总数。
         */
        _message_size = _message_end,
    };
} // namespace peripheral
