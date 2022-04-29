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

        _message_end,
    };
} // namespace peripheral
