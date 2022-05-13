/**
 * @file msg_data.hpp
 * @author UnnamedOrange
 * @brief 提取 std::pair 格式的消息的额外数据。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <memory>
#include <tuple>
#include <utility>

namespace utils
{
    /**
     * @brief 提取 std::pair 格式的消息的额外数据。
     *
     * @note 注意返回的是常值引用。
     *
     * @tparam T 消息的数据类型。
     * @param pair std::pair 格式的消息。
     */
    template <typename T, typename pair_t>
    const T& msg_data(const pair_t& pair)
    {
        return *std::static_pointer_cast<T>(pair.second);
    }
} // namespace utils
