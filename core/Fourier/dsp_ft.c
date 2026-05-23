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

/* Private Enums -------------------------------------------------------------*/

/* Private Typedefs ----------------------------------------------------------*/

/* Private Structs -----------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/

/* Public Function Implementations -------------------------------------------*/

/**
 * @brief  Computes the Discrete Fourier Transform (DFT) of a complex-valued
 *         input signal.
 *
 *         X[k] = Σ (n=0 to N-1) x[n] * e^(-j * 2π * k * n / N)
 *
 *         where x[n] = src_signal_re[n] + j * src_signal_im[n]:
 *
 *         Re{X[k]} =  Σ [x_re[n] * cos(2πkn/N) + x_im[n] * sin(2πkn/N)]
 *         Im{X[k]} =  Σ [-x_re[n] * sin(2πkn/N) + x_im[n] * cos(2πkn/N)]
 *
 * @param  ip_signal  Pointer to a dsp_FTsignal_t struct where:
 *                 - ip_signal->src_signal_re is the input real part
 *                   (length = size, must not be NULL)
 *                 - ip_signal->src_signal_im is the input imaginary part
 *                   (length = size, set to zeros for real-only signals)
 *                 - ip_signal->re     is the output real part      (length =
 *                   size)
 *                 - ip_signal->im     is the output imaginary part (length =
 *                   size)
 *                 - ip_signal->size   is N, the number of samples
 */
void dsp_dft_calc(dsp_FTsignal_t *ip_signal) {
  int N = ip_signal->size;

  if (ip_signal->src_signal_re == NULL) {
    return;
  }

  for (int k = 0; k < N; k++) {
    ip_signal->re[k] = 0.0;
    ip_signal->im[k] = 0.0;

    for (int n = 0; n < N; n++) {
      double angle = (2.0 * PI * k * n) / N;
      ip_signal->re[k] += ip_signal->src_signal_re[n] * cos(angle) +
                          ip_signal->src_signal_im[n] * sin(angle);
      ip_signal->im[k] += -ip_signal->src_signal_re[n] * sin(angle) +
                          ip_signal->src_signal_im[n] * cos(angle);
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
 * @param  ip_signal  Pointer to a dsp_FTsignal_t struct where:
 *                 - ip_signal->re     is the input real part      (length =
 * size)
 *                 - ip_signal->im     is the input imaginary part (length =
 * size)
 *                 - ip_signal->src_signal_re is the output time-domain array
 * (length = size)
 *                 - ip_signal->size   is N, the number of samples
 */
void dsp_idft_calc(dsp_FTsignal_t *ip_signal) {
  int N = ip_signal->size;

  for (int n = 0; n < N; n++) {
    ip_signal->src_signal_re[n] = 0.0;

    for (int k = 0; k < N; k++) {
      double angle = (2.0 * PI * k * n) / N;
      ip_signal->src_signal_re[n] +=
          ip_signal->re[k] * cos(angle) - ip_signal->im[k] * sin(angle);
    }

    ip_signal->src_signal_re[n] /= N;
  }
}

/**
 * @brief  Recursive in-place radix-2 Cooley-Tukey butterfly.
 *
 * @param  re   Real part array (modified in-place)
 * @param  im   Imaginary part array (modified in-place)
 * @param  N    Number of samples (must be a power of 2)
 * @param  step Current stride between elements
 */
static void fft_recurse(double *re, double *im, int N, int step) {
  if (N <= 1) {
    return;
  }

  int half = N / 2;

  /* Recurse on even and odd halves */
  fft_recurse(re, im, half, step * 2);
  fft_recurse(re + step, im + step, half, step * 2);

  for (int k = 0; k < half; k++) {
    int even_idx = k * step * 2;
    int odd_idx = even_idx + step;

    double angle = -2.0 * PI * k / N;
    double twiddle_re = cos(angle);
    double twiddle_im = sin(angle);

    /* Butterfly: multiply odd element by twiddle factor */
    double t_re = twiddle_re * re[odd_idx] - twiddle_im * im[odd_idx];
    double t_im = twiddle_re * im[odd_idx] + twiddle_im * re[odd_idx];

    double e_re = re[even_idx];
    double e_im = im[even_idx];

    re[even_idx] = e_re + t_re;
    im[even_idx] = e_im + t_im;
    re[odd_idx] = e_re - t_re;
    im[odd_idx] = e_im - t_im;
  }
}

/**
 * @brief  Computes the Fast Fourier Transform (FFT) using the radix-2
 *         Cooley-Tukey decimation-in-time algorithm.
 *
 *         X[k] = Σ (n=0 to N-1) x[n] * e^(-j * 2π * k * n / N)
 *
 *         Complexity: O(N log N) compared to O(N²) for the DFT.
 *
 * @note   ip_signal->size MUST be a power of 2.
 *
 * @param  ip_signal  Pointer to a dsp_FTsignal_t struct where:
 *                 - ip_signal->src_signal_re is the input real part
 *                   (length = size, must not be NULL)
 *                 - ip_signal->src_signal_im is the input imaginary part
 *                   (length = size, set to zeros for real-only signals)
 *                 - ip_signal->re     is the output real part      (length =
 *                   size)
 *                 - ip_signal->im     is the output imaginary part (length =
 *                   size)
 *                 - ip_signal->size   is N, the number of samples
 */
void dsp_fft_calc(dsp_FTsignal_t *ip_signal) {
  int N = ip_signal->size;

  if (ip_signal->src_signal_re == NULL) {
    return;
  }

  /* Copy source signal into output arrays for in-place computation */
  for (int i = 0; i < N; i++) {
    ip_signal->re[i] = ip_signal->src_signal_re[i];
    ip_signal->im[i] = ip_signal->src_signal_im[i];
  }

  /* Bit-reversal permutation */
  int bits = 0;
  int temp = N;
  while (temp > 1) {
    bits++;
    temp >>= 1;
  }

  for (int i = 0; i < N; i++) {
    int reversed = 0;
    for (int b = 0; b < bits; b++) {
      reversed |= ((i >> b) & 1) << (bits - 1 - b);
    }
    if (reversed > i) {
      double tmp_re = ip_signal->re[i];
      double tmp_im = ip_signal->im[i];
      ip_signal->re[i] = ip_signal->re[reversed];
      ip_signal->im[i] = ip_signal->im[reversed];
      ip_signal->re[reversed] = tmp_re;
      ip_signal->im[reversed] = tmp_im;
    }
  }

  /* Iterative butterfly stages */
  for (int stage_size = 2; stage_size <= N; stage_size *= 2) {
    int half = stage_size / 2;
    for (int k = 0; k < N; k += stage_size) {
      for (int j = 0; j < half; j++) {
        double angle = -2.0 * PI * j / stage_size;
        double twiddle_re = cos(angle);
        double twiddle_im = sin(angle);

        int even_idx = k + j;
        int odd_idx = k + j + half;

        double t_re = twiddle_re * ip_signal->re[odd_idx] -
                      twiddle_im * ip_signal->im[odd_idx];
        double t_im = twiddle_re * ip_signal->im[odd_idx] +
                      twiddle_im * ip_signal->re[odd_idx];

        double e_re = ip_signal->re[even_idx];
        double e_im = ip_signal->im[even_idx];

        ip_signal->re[even_idx] = e_re + t_re;
        ip_signal->im[even_idx] = e_im + t_im;
        ip_signal->re[odd_idx] = e_re - t_re;
        ip_signal->im[odd_idx] = e_im - t_im;
      }
    }
  }
}
