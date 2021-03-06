/**
 * \file rcc.h
 * \brief Header for rcc.cpp
 * \author: Mazeryt Freager
 * \date 2012-08-30
 */

#ifndef RCC_H_
#define RCC_H_

#include <stdint.h>

#include "hdr/hdr_rcc.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global variables' types
+---------------------------------------------------------------------------------------------------------------------*/

/// enum that lists all possible PLL input sources
enum rccPllInput
{
	RCC_PLL_INPUT_HSI = RCC_CFGR_PLLSRC_HSI_value, RCC_PLL_INPUT_HSE = RCC_CFGR_PLLSRC_HSE_value,
};

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

uint32_t rccStartPll(enum rccPllInput pll_input, uint32_t input_frequency, uint32_t output_frequency);

#ifdef __cplusplus
extern "C" {
#endif

uint32_t rccGetCoreFrequency(void);

#ifdef __cplusplus
}
#endif

#endif /* RCC_H_ */
