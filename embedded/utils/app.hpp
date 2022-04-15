/**
 * @file app.hpp
 * @author UnnamedOrange
 * @brief 将类视作应用程序。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <type_traits>

namespace utils
{
    /**
     * @brief 调用一个类的构造函数，将这个类看作一个应用程序。
     *
     * @tparam app_t 应用程序类的类名。
     * @param r 构造函数的参数。
     */
    template <typename app_t, typename... R>
    inline void run_app(R&&... r)
    {
        delete new app_t(std::forward<R>(r)...);
    }
} // namespace utils
