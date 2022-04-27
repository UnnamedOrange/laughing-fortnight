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

        _message_end,
        /**
         * @brief 被定义的消息的总数。
         */
        _message_size = _message_end,
    };
} // namespace peripheral
