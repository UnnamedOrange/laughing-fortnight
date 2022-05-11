/**
 * @file clock_config.cpp
 * @author UnnamedOrange
 * @brief 覆盖默认的时钟初始化函数。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

// 仅作用于 NUCLEO L476RG 开发板。
#ifdef TARGET_NUCLEO_L476RG

#include "mbed.h"

extern "C"
{
    /**
     * @brief 将主时钟修改为 2MHz。
     * 覆盖 MBed 的弱符号 SetSysClock_PLL_MSI。
     *
     * @note 考虑到外设，频率不能再减小。
     *
     * @note 该函数删除了配置 USB 的功能。
     */
    uint8_t SetSysClock_PLL_MSI(void)
    {
        RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
        RCC_OscInitTypeDef RCC_OscInitStruct = {0};
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

#if MBED_CONF_TARGET_LSE_AVAILABLE
        // Enable LSE Oscillator to automatically calibrate the MSI clock
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // No PLL update
        RCC_OscInitStruct.LSEState =
            RCC_LSE_ON; // External 32.768 kHz clock on OSC32_IN/OSC32_OUT
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            return 0; // FAIL
        }
#endif /* MBED_CONF_TARGET_LSE_AVAILABLE */

        /* Enable MSI Oscillator and activate PLL with MSI as source */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
        RCC_OscInitStruct.MSIState = RCC_MSI_ON;
        RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
        RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6; // 4 MHz
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
        RCC_OscInitStruct.PLL.PLLM = 1;  // 4 MHz
        RCC_OscInitStruct.PLL.PLLN = 16; // 64 MHz
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
        RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
        RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV8; // 8 MHz
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            return 0; // FAIL
        }

#if MBED_CONF_TARGET_LSE_AVAILABLE
        /* Enable MSI Auto-calibration through LSE */
        HAL_RCCEx_EnableMSIPLLMode();
#endif /* MBED_CONF_TARGET_LSE_AVAILABLE */

        // Select PLL as system clock source and configure the HCLK, PCLK1 and
        // PCLK2 clocks dividers
        RCC_ClkInitStruct.ClockType =
            (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
             RCC_CLOCKTYPE_PCLK2);
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // 8 MHz
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;        // 2 MHz
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;         // 2 MHz
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;         // 2 MHz
        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
        {
            return 0; // FAIL
        }

        // Output clock on MCO1 pin(PA8) for debugging purpose
#if DEBUG_MCO == 4
        HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_MSI, RCC_MCODIV_2); // 2 MHz
#endif

        // https://forums.mbed.com/t/how-to-change-system-clock-frequency-on-nucleo-l476rg/16142/8
        // 最好加上。
        SystemCoreClockUpdate();
        return 1; // OK
    }
}

#endif
