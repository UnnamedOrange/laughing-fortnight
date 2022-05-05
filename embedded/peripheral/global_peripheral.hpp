/**
 * @file global_peripheral.hpp
 * @author UnnamedOrange
 * @brief 外设接口管脚定义。
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
/**
 * @brief 加速度计的 MOSI 管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_MOSI = NC;
/**
 * @brief 加速度计的 MISO 管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_MISO = NC;
/**
 * @brief 加速度计的 SCLK 管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_SCLK = NC;
/**
 * @brief 加速度计的 CS 管脚。
 *
 * @note 在具体硬件上修改该常量。
 *
 * @todo 使用设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_CS = NC;

#pragma GCC diagnostic pop
