/**
 * @file gps_message.hpp
 * @author UnnamedOrange
 * @brief 定义 GPS 的消息。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

namespace peripheral
{
    enum class gps_message_enum_t : int
    {
        _message_begin,

        /**
         * @brief 初始化。
         */
        init,
        /**
         * @brief 请求在位置信息更新时通知外部队列。
         */
        request_notify,

        _message_end,
        /**
         * @brief 被定义的消息的总数。
         */
        _message_size = _message_end,
    };
} // namespace peripheral
