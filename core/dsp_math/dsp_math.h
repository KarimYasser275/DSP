/**
 * @file      dsp_math.h
 * @brief     Brief description of this module
 * @author    Karim Yasser
 * @date      2026-05-09
 * @copyright Copyright (c) 2026 Company Name
 */

#ifndef DSP_MATH_H
#define DSP_MATH_H

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
 * @brief  Converts Cartesian (rectangular) coordinates to polar form.
 * @param[in,out] ip_signal  Pointer to dsp_cartPolar_t with re/im as input,
 *                        mag/phase as output.
 */
void dsp_cart2Polar(dsp_cartPolar_t *ip_signal);

/**
 * @brief  Converts polar coordinates to Cartesian (rectangular) form.
 * @param[in,out] ip_signal  Pointer to dsp_cartPolar_t with mag/phase as input,
 *                        re/im as output.
 */
void dsp_polar2Cart(dsp_cartPolar_t *ip_signal);

#ifdef __cplusplus
}
#endif

#endif /* DSP_MATH_H */