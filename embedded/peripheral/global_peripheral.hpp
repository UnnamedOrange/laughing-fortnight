/**
 * @file global_peripheral.hpp
 * @author UnnamedOrange
 * @brief 外设接口全局变量。由于某些接口必须指定双工工作模式，
 * 所以需要定义接口全局变量。修改以下常量：
 * - @ref PIN_BC26_TX
 * - @ref PIN_BC26_RX
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++17-extensions"
/**
 * @brief BC26 的 TX（CPU 端）管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_BC26_TX = USBTX;
/**
 * @brief BC26 的 RX（CPU 端）管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_GPS_RX = USBRX;
/**
 * @brief GPS 的 TX（CPU 端）管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_GPS_TX = USBTX;
/**
 * @brief GPS 的 RX（CPU 端）管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_BC26_RX = USBRX;

inline mbed::BufferedSerial serial_bc26{PIN_BC26_TX, PIN_BC26_RX};
inline mbed::BufferedSerial serial_gps{PIN_GPS_TX, PIN_GPS_RX};
#pragma GCC diagnostic pop
