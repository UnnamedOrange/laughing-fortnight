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
         * @brief BC26 模块 send_at 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         */
        bc26_send_at,
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
         */
        bc26_send_at_cimi,
        /**
         * @brief BC26 模块 send_at_cgatt_get 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         */
        bc26_send_at_cgatt_get,
        /**
         * @brief BC26 模块 send_at_cesq 的反馈消息。
         *
         * @param bool 是否成功收到 OK。
         */
        bc26_send_at_cesq,

        _message_end,
    };
} // namespace peripheral
