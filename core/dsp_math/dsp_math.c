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
 * @param[in,out] ip_signal  Pointer to a dsp_cartPolar_t struct where:
 *                        - ip_signal->re    (input)  real part array
 *                        - ip_signal->im    (input)  imaginary part array
 *                        - ip_signal->mag   (output) magnitude array
 *                        - ip_signal->phase (output) phase angle array
 * (radians)
 *                        - ip_signal->size  number of elements
 */
void dsp_cart2Polar(dsp_cartPolar_t *ip_signal) {
  for (int i = 0; i < ip_signal->size; i++) {
    ip_signal->mag[i] = sqrt((ip_signal->re[i] * ip_signal->re[i]) +
                             (ip_signal->im[i] * ip_signal->im[i]));
    ip_signal->phase[i] = atan2(ip_signal->im[i], ip_signal->re[i]);
  }
}

/**
 * @brief  Converts polar coordinates to Cartesian (rectangular) form.
 *
 *         re[i] = mag[i] * cos(phase[i])
 *         im[i] = mag[i] * sin(phase[i])
 *
 * @param[in,out] ip_signal  Pointer to a dsp_cartPolar_t struct where:
 *                        - ip_signal->mag   (input)  magnitude array
 *                        - ip_signal->phase (input)  phase angle array
 * (radians)
 *                        - ip_signal->re    (output) real part array
 *                        - ip_signal->im    (output) imaginary part array
 *                        - ip_signal->size  number of elements
 */
void dsp_polar2Cart(dsp_cartPolar_t *ip_signal) {
  for (int i = 0; i < ip_signal->size; i++) {
    ip_signal->re[i] = ip_signal->mag[i] * cos(ip_signal->phase[i]);
    ip_signal->im[i] = ip_signal->mag[i] * sin(ip_signal->phase[i]);
  }
}

/* Private Function Implementations ------------------------------------------*/
