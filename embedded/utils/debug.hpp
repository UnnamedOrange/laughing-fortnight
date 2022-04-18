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

namespace utils
{
#if DEVICE_STDIO_MESSAGES && !defined(NDEBUG)
    namespace details
    {
        inline rtos::Semaphore _debug_semaphore{1, 1};
    }
#endif
    /**
     * @brief 在 NDEBUG 宏未被定义时调试输出。
     *
     * @note 相比 Mbed 的 debug 函数，本函数额外加锁了。
     *
     * @param format 格式化字符串。
     * @param ... 参数。
     */
    inline void debug_printf(const char* format, ...)
    {
#if DEVICE_STDIO_MESSAGES && !defined(NDEBUG)
        details::_debug_semaphore.acquire();
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        details::_debug_semaphore.release();
#endif
    }
} // namespace utils
