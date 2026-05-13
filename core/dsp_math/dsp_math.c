/**
 * @file      dsp_math.c
 * @brief     Implementation of the module
 * @author    Karim Yasser
 * @date      2026-05-09
 * @copyright Copyright (c) 2026 Company Name
 */

/* Includes ------------------------------------------------------------------*/
#include "dsp_math.h"
#include <math.h>
/* Private Macros ------------------------------------------------------------*/
#define PI 3.14159265358979f
/* Private Enums -------------------------------------------------------------*/

/* Private Typedefs ----------------------------------------------------------*/

/* Private Structs -----------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/

/* Public Function Implementations -------------------------------------------*/

/**
 * @brief  Converts Cartesian (rectangular) coordinates to polar form.
 *
 *         mag[i]   = sqrt(re[i]^2 + im[i]^2)
 *         phase[i] = atan2(im[i], re[i])
 *
 * @param[in,out] signal  Pointer to a dsp_cartPolar_t struct where:
 *                        - signal->re    (input)  real part array
 *                        - signal->im    (input)  imaginary part array
 *                        - signal->mag   (output) magnitude array
 *                        - signal->phase (output) phase angle array (radians)
 *                        - signal->size  number of elements
 */
void dsp_cart2Polar(dsp_cartPolar_t *signal) {
  for (int i = 0; i < signal->size; i++) {
    signal->mag[i] =
        sqrt((signal->re[i] * signal->re[i]) + (signal->im[i] * signal->im[i]));
    signal->phase[i] = atan2(signal->im[i], signal->re[i]);
  }
}

/**
 * @brief  Converts polar coordinates to Cartesian (rectangular) form.
 *
 *         re[i] = mag[i] * cos(phase[i])
 *         im[i] = mag[i] * sin(phase[i])
 *
 * @param[in,out] signal  Pointer to a dsp_cartPolar_t struct where:
 *                        - signal->mag   (input)  magnitude array
 *                        - signal->phase (input)  phase angle array (radians)
 *                        - signal->re    (output) real part array
 *                        - signal->im    (output) imaginary part array
 *                        - signal->size  number of elements
 */
void dsp_polar2Cart(dsp_cartPolar_t *signal) {
  for (int i = 0; i < signal->size; i++) {
    signal->re[i] = signal->mag[i] * cos(signal->phase[i]);
    signal->im[i] = signal->mag[i] * sin(signal->phase[i]);
  }
}

/* Private Function Implementations ------------------------------------------*/
