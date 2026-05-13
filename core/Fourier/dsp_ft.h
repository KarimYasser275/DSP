/**
 * @file      dsp_ft.h
 * @brief     Brief description of this module
 * @author    Karim Yasser
 * @date      2026-05-09
 * @copyright Copyright (c) 2026 Company Name
 */

#ifndef DSP_FT_H
#define DSP_FT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../signals/dsp_signal_types.h"
#include <stdbool.h>
#include <stdint.h>

/* Macros --------------------------------------------------------------------*/
/* Enums ---------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Structs -------------------------------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Function Prototypes -------------------------------------------------------*/

/**
 * @brief  Brief description of the function
 * @param  param_name Description of parameter
 * @return Description of return value
 */
void dsp_dft_calc(dsp_FTsignal_t *signal);

/**
 * @brief  Computes the Inverse Discrete Fourier Transform (IDFT).
 * @param  signal  Pointer to a dsp_FTsignal_t struct where:
 *                 - signal->re     is the input real part      (length = size)
 *                 - signal->im     is the input imaginary part (length = size)
 *                 - signal->signal is the output time-domain array (length =
 * size)
 *                 - signal->size   is N, the number of samples
 */
void dsp_idft_calc(dsp_FTsignal_t *signal);

#ifdef __cplusplus
}
#endif

#endif /* DSP_FT_H */