/**
 * @file      dsp_ft.c
 * @brief     Implementation of the module
 * @author    Karim Yasser
 * @date      2026-05-09
 * @copyright Copyright (c) 2026 Company Name
 */

/* Includes ------------------------------------------------------------------*/
#include "dsp_ft.h"
#include <math.h>

/* Private Macros ------------------------------------------------------------*/
#define PI 3.14159265358979

/* Private Enums -------------------------------------------------------------*/

/* Private Typedefs ----------------------------------------------------------*/

/* Private Structs -----------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/

/* Public Function Implementations -------------------------------------------*/

/**
 * @brief  Computes the Discrete Fourier Transform (DFT) of a real-valued
 * signal.
 *
 *         X[k] = Σ (n=0 to N-1) x[n] * e^(-j * 2π * k * n / N)
 *
 *         Re{X[k]} =  Σ x[n] * cos(2π * k * n / N)
 *         Im{X[k]} = -Σ x[n] * sin(2π * k * n / N)
 *
 * @param  signal  Pointer to a dsp_FTsignal_t struct where:
 *                 - signal->signal is the input time-domain array (length =
 * size)
 *                 - signal->re     is the output real part      (length = size)
 *                 - signal->im     is the output imaginary part (length = size)
 *                 - signal->size   is N, the number of samples
 */
void dsp_dft_calc(dsp_FTsignal_t *signal) {
  int N = signal->size;

  for (int k = 0; k < N; k++) {
    signal->re[k] = 0.0;
    signal->im[k] = 0.0;

    for (int n = 0; n < N; n++) {
      double angle = (2.0 * PI * k * n) / N;
      signal->re[k] += signal->signal[n] * cos(angle);
      signal->im[k] -= signal->signal[n] * sin(angle);
    }
  }
}

/**
 * @brief  Computes the Inverse Discrete Fourier Transform (IDFT).
 *
 *         x[n] = (1/N) * Σ (k=0 to N-1) X[k] * e^(j * 2π * k * n / N)
 *
 *         x[n] = (1/N) * Σ [Re{X[k]}*cos(2πkn/N) - Im{X[k]}*sin(2πkn/N)]
 *
 * @param  signal  Pointer to a dsp_FTsignal_t struct where:
 *                 - signal->re     is the input real part      (length = size)
 *                 - signal->im     is the input imaginary part (length = size)
 *                 - signal->signal is the output time-domain array (length =
 * size)
 *                 - signal->size   is N, the number of samples
 */
void dsp_idft_calc(dsp_FTsignal_t *signal) {
  int N = signal->size;

  for (int n = 0; n < N; n++) {
    signal->signal[n] = 0.0;

    for (int k = 0; k < N; k++) {
      double angle = (2.0 * PI * k * n) / N;
      signal->signal[n] +=
          signal->re[k] * cos(angle) - signal->im[k] * sin(angle);
    }

    signal->signal[n] /= N;
  }
}

/* Private Function Implementations ------------------------------------------*/
