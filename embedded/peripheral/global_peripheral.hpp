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
 */
inline constexpr PinName PIN_BC26_TX = PA_9;
/**
 * @brief BC26 的 RX（CPU 端）管脚。
 */
inline constexpr PinName PIN_BC26_RX = PA_10;

/**
 * @brief GPS 的电源使能管脚。
 *
 * @todo 检查是高使能还是低使能。
 */
inline constexpr PinName PIN_GPS_EN = PB_3;
/**
 * @brief GPS 的 TX（CPU 端）管脚。
 */
inline constexpr PinName PIN_GPS_TX = PC_4;
/**
 * @brief GPS 的 RX（CPU 端）管脚。
 */
inline constexpr PinName PIN_GPS_RX = PC_5;

/**
 * @brief 加速度计的 MOSI 管脚。
 *
 * @todo 检查设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_MOSI = PA_7;
/**
 * @brief 加速度计的 MISO 管脚。
 *
 * @todo 检查设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_MISO = PA_6;
/**
 * @brief 加速度计的 SCLK 管脚。
 *
 * @todo 检查设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_SCLK = PA_5;
/**
 * @brief 加速度计的 CS 管脚。
 *
 * @todo 检查设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_CS = PC_11;
/**
 * @brief 加速度计的中断 1 管脚。
 *
 * @todo 检查设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_INT1 = PC_10;
/**
 * @brief 加速度计的中断 2 管脚。
 *
 * @todo 检查设计的管脚。
 */
inline constexpr PinName PIN_ACCEL_INT2 = PC_12;

/**
 * @brief 蜂鸣器的电源使能管脚。是 5 V 稳压模块的使能。
 *
 * @todo 检查是高使能还是低使能。
 */
inline constexpr PinName PIN_BUZZER_EN = PA_8;
/**
 * @brief 蜂鸣器的输出管脚。
 */
inline constexpr PinName PIN_BUZZER = PB_10;

/**
 * @brief LED 灯的管脚。
 *
 * @todo 检查是高使能还是低使能。
 */
inline constexpr PinName PIN_LED = PB_5;

#pragma GCC diagnostic pop
