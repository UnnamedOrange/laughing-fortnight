/**
 * @file buzzer_message.hpp
 * @author UnnamedOrange
 * @brief 定义蜂鸣器的消息。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

namespace peripheral
{
    enum class buzzer_message_enum_t : int
    {
        _message_begin,

        /**
         * @brief 蜂鸣器响一会儿。
         */
        buzz,

        _message_end,
        /**
         * @brief 被定义的消息的总数。
         */
        _message_size = _message_end,
    };
} // namespace peripheral
