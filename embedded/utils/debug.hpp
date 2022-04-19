/**
 * @file debug.hpp
 * @author UnnamedOrange
 * @brief 调试工具函数。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#ifdef USE_TFT_FOR_DEBUG_CONSOLE
#include <tft/tft_debug_console.hpp>
#endif

namespace utils
{
#if DEVICE_STDIO_MESSAGES && !defined(NDEBUG)
    namespace details
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++17-extensions"
        inline rtos::Semaphore _debug_semaphore{1, 1};
#ifdef USE_TFT_FOR_DEBUG_CONSOLE
        inline modules::tft_debug_console _debug_console;
#pragma GCC diagnostic pop
#endif
    } // namespace details
#endif
    /**
     * @brief 在 NDEBUG 宏未被定义时调试输出。
     *
     * @note 相比 Mbed 的 debug 函数，本函数额外加锁了。
     *
     * @param format 格式化字符串。
     * @param ... 参数。
     */
    template <typename... R>
    inline void debug_printf(const char* format, R&&... args)
    {
#if DEVICE_STDIO_MESSAGES && !defined(NDEBUG)
        details::_debug_semaphore.acquire();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
        debug(format, std::forward<R>(args)...);
#pragma GCC diagnostic pop
#ifdef USE_TFT_FOR_DEBUG_CONSOLE
        details::_debug_console.printf(format, std::forward<R>(args)...);
#endif
        details::_debug_semaphore.release();
#endif
    }
} // namespace utils
