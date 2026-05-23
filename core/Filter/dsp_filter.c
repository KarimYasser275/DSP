/**
 * @file      dsp_filter.c
 * @brief     Windowed-sinc FIR low-pass and high-pass filter implementation
 * @author    Karim Yasser
 * @date      2026-05-23
 * @copyright Copyright (c) 2026 Company Name
 */

/* Includes ------------------------------------------------------------------*/
#include "dsp_filter.h"
#include <math.h>

/* Private Macros ------------------------------------------------------------*/
#ifndef PI
#define PI 3.14159265358979
#endif

/* Private Enums -------------------------------------------------------------*/

/* Private Typedefs ----------------------------------------------------------*/

/* Private Structs -----------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/
static void build_lpf_kernel(double *kernel, int kernel_len, double fc);
static void convolve(const dsp_filter_t *cfg);

/* Public Function Implementations -------------------------------------------*/

/**
 * @brief  Apply a windowed-sinc FIR low-pass filter.
 *
 * @pre cfg->fc         : (0.0, 0.5)  normalised cutoff
 * @pre cfg->kernel_len : odd, >= 3
 * @pre cfg->input_len  : >= 1
 * @pre cfg->input      : allocated [input_len]
 * @pre cfg->output     : allocated [input_len + kernel_len - 1]
 * @pre cfg->kernel     : allocated [kernel_len]
 */
void dsp_LPF(dsp_filter_t *cfg) {
  /* Build the ideal sinc LPF kernel, shaped by a Blackman window */
  build_lpf_kernel(cfg->kernel, cfg->kernel_len, cfg->fc);

  /* Apply the filter by convolving the input signal with the kernel */
  convolve(cfg);
}

/**
 * @brief  Apply a windowed-sinc FIR high-pass filter.
 *
 * @pre cfg->fc         : (0.0, 0.5)  normalised cutoff
 * @pre cfg->kernel_len : odd, >= 3
 * @pre cfg->input_len  : >= 1
 * @pre cfg->input      : allocated [input_len]
 * @pre cfg->output     : allocated [input_len + kernel_len - 1]
 * @pre cfg->kernel     : allocated [kernel_len]
 */
void dsp_HPF(dsp_filter_t *cfg) {
  /* Start with the same LPF sinc kernel */
  build_lpf_kernel(cfg->kernel, cfg->kernel_len, cfg->fc);

  /* Spectral inversion converts an LPF into an HPF:
   *   H_hp(z) = delta(z) - H_lp(z)
   * In the time domain this means: negate every tap, then add 1
   * to the centre tap (the delta impulse position).                */
  int centre = cfg->kernel_len / 2;
  for (int i = 0; i < cfg->kernel_len; i++) {
    cfg->kernel[i] = -cfg->kernel[i];
  }
  cfg->kernel[centre] += 1.0;

  /* Convolve the input with the resulting HPF kernel */
  convolve(cfg);
}

/**
 * @brief  Apply a windowed-sinc FIR band-pass filter.
 *
 * @pre cfg->fc         : (0.0, 0.5)  lower normalised cutoff
 * @pre cfg->fc_high    : (fc, 0.5)   upper normalised cutoff, must be > fc
 * @pre cfg->kernel_len : odd, >= 3
 * @pre cfg->input_len  : >= 1
 * @pre cfg->input      : allocated [input_len]
 * @pre cfg->output     : allocated [input_len + kernel_len - 1]
 * @pre cfg->kernel     : allocated [kernel_len]
 */
void dsp_BPF(dsp_filter_t *cfg) {
  /* BPF = LPF(fc_high) − LPF(fc_low)
   * Subtracting a narrower LPF from a wider one leaves only the band
   * between the two cutoff frequencies.                               */

  /* 1. Build the upper-cutoff LPF kernel → store in cfg->kernel */
  build_lpf_kernel(cfg->kernel, cfg->kernel_len, cfg->fc_high);

  /* 2. Build the lower-cutoff LPF kernel → store temporarily in the
   *    first kernel_len elements of cfg->output (scratch space that
   *    convolve() will overwrite later).                              */
  build_lpf_kernel(cfg->output, cfg->kernel_len, cfg->fc);

  /* 3. Subtract element-wise: kernel = LPF_high − LPF_low */
  for (int i = 0; i < cfg->kernel_len; i++) {
    cfg->kernel[i] -= cfg->output[i];
  }

  /* 4. Convolve the input with the resulting BPF kernel */
  convolve(cfg);
}

/**
 * @brief  Initialise a 1st-order IIR filter and compute its coefficients.
 *
 * @pre filt->fc  : (0, filt->fs / 2)  cutoff in Hz, must be < Nyquist
 * @pre filt->fs  : > 0                sampling frequency in Hz
 */
void dsp_1stOrderFilterInit(dsp_1stOrderFilter_t *filt) {
  /* Clear filter memory so it starts from a zero-state */
  filt->x_prev = 0.0;
  filt->y_prev = 0.0;

  /* Map the analog RC pole to the digital domain:
   *   x = e^(-2π · fc / fs)
   * x is the pole radius; closer to 1 = heavier smoothing           */
  double x = exp(-2.0 * PI * filt->fc / filt->fs);

  if (filt->is_hpf) {
    /* HPF coefficients — derived by subtracting the LPF from
     * an all-pass: H_hp(z) = 1 - H_lp(z)
     *   a0 =  (1 + x) / 2
     *   a1 = -(1 + x) / 2                                         */
    filt->a0 = (1.0 + x) / 2.0;
    filt->a1 = -(1.0 + x) / 2.0;
  } else {
    /* LPF coefficients — single-pole exponential smoothing:
     *   a0 = 1 - x   (gain for new sample)
     *   a1 = 0        (no delayed-input term)                     */
    filt->a0 = 1.0 - x;
    filt->a1 = 0.0;
  }

  /* Feedback coefficient is the same for both LPF and HPF */
  filt->b1 = -x;
}

/**
 * @brief  Process one sample through a 1st-order IIR filter.
 *
 * @pre filt : previously initialised via dsp_1stOrderFilterInit()
 * @pre input: any finite double value
 */
double dsp_1stOrderFilterCalc(dsp_1stOrderFilter_t *filt, double input) {
  /* 1st-order IIR difference equation:
   *   y[n] = a0·x[n] + a1·x[n-1] - b1·y[n-1]                       */
  double output =
      filt->a0 * input + filt->a1 * filt->x_prev - filt->b1 * filt->y_prev;

  /* Shift the delay line for the next sample */
  filt->x_prev = input;
  filt->y_prev = output;

  return output;
}

/**
 * @brief  Initialise a Q15 fixed-point 1st-order IIR filter.
 *
 * @pre filt->fc  : (0, filt->fs / 2)  cutoff in Hz, must be < Nyquist
 * @pre filt->fs  : > 0                sampling frequency in Hz
 * @note Coefficients are in range (-1.0, 1.0) and fit in Q15 without overflow
 *       as long as fc > 0 and fc < fs/2.
 */
void dsp_1stOrderFilterFPInit(dsp_1stOrderFilterFP_t *filt) {
  /* Clear the delay line */
  filt->x_prev = 0;
  filt->y_prev = 0;

  /* Compute the pole in floating-point (same maths as the double version) */
  double x = exp(-2.0 * PI * filt->fc / filt->fs);

  /* Compute floating-point coefficients first */
  double a0_f, a1_f, b1_f;

  if (filt->is_hpf) {
    /* HPF: a0 = (1+x)/2, a1 = -(1+x)/2 */
    a0_f = (1.0 + x) / 2.0;
    a1_f = -(1.0 + x) / 2.0;
  } else {
    /* LPF: a0 = 1-x, a1 = 0 */
    a0_f = 1.0 - x;
    a1_f = 0.0;
  }
  b1_f = -x;

  /* Convert to Q15: multiply by 2^15 (32768) and truncate to int16_t.
   * All coefficients lie in (-1, 1) so the result fits without overflow. */
  filt->a0 = (int16_t)(a0_f * DSP_Q15_SCALE);
  filt->a1 = (int16_t)(a1_f * DSP_Q15_SCALE);
  filt->b1 = (int16_t)(b1_f * DSP_Q15_SCALE);
}

/**
 * @brief  Process one sample through a Q15 fixed-point 1st-order IIR filter.
 *
 * @pre filt  : previously initialised via dsp_1stOrderFilterFPInit()
 * @pre input : [-32768, 32767]  (full int16_t range)
 * @note Accumulator fits in int32_t: worst case 3 × 32768 × 32768 < 2^31.
 *       Output stays within int16_t range for stable filter coefficients.
 */
int16_t dsp_1stOrderFilterFPCalc(dsp_1stOrderFilterFP_t *filt, int16_t input) {
  /* Each product is Q15 × int16 = Q30 (30 fractional bits).
   * Sum of 3 products still fits in int32_t (max ≈ 3·2^30 < 2^31).  */
  int32_t acc = (int32_t)filt->a0 * (int32_t)input +
                (int32_t)filt->a1 * (int32_t)filt->x_prev -
                (int32_t)filt->b1 * (int32_t)filt->y_prev;

  /* Right-shift by 15 to discard the fractional bits → back to int16 */
  int16_t output = (int16_t)(acc >> DSP_Q15_SHIFT);

  /* Advance the delay line */
  filt->x_prev = input;
  filt->y_prev = output;

  return output;
}

/* Private Function Implementations ------------------------------------------*/

/**
 * @brief  Build a normalised windowed-sinc low-pass kernel (Blackman window).
 *
 * @pre kernel     : allocated [kernel_len]
 * @pre kernel_len : odd, >= 3
 * @pre fc         : (0.0, 0.5)
 */
static void build_lpf_kernel(double *kernel, int kernel_len, double fc) {
  int M = kernel_len - 1; /* filter order (number of taps - 1)          */
  int centre = M / 2;     /* index of the centre (peak) of the sinc     */
  double sum = 0.0;       /* running sum for DC-gain normalisation       */

  for (int i = 0; i <= M; i++) {
    /* Ideal sinc low-pass impulse response:
     *   h[n] = sin(2π·fc·n) / n        for n ≠ 0
     *   h[0] = 2π·fc                   (L'Hôpital limit at n = 0)  */
    if (i == centre) {
      kernel[i] = 2.0 * PI * fc;
    } else {
      double n = (double)(i - centre);
      kernel[i] = sin(2.0 * PI * fc * n) / n;
    }

    /* Blackman window — smoothly tapers the sinc to zero at edges,
     * giving ~74 dB stop-band attenuation:
     *   w[i] = 0.42 - 0.50·cos(2πi/M) + 0.08·cos(4πi/M)          */
    double w = 0.42 - 0.5 * cos(2.0 * PI * (double)i / (double)M) +
               0.08 * cos(4.0 * PI * (double)i / (double)M);
    kernel[i] *= w;
    sum += kernel[i];
  }

  /* Normalise the kernel so that the DC gain is exactly 1.0 */
  for (int i = 0; i <= M; i++) {
    kernel[i] /= sum;
  }
}

/**
 * @brief  Convolve input with kernel → output.
 *
 * @pre cfg->input      : allocated [input_len],  input_len >= 1
 * @pre cfg->kernel     : filled    [kernel_len], kernel_len >= 1
 * @pre cfg->output     : allocated [input_len + kernel_len - 1]
 * @note Samples outside the input range are treated as zero (zero-padding).
 */
static void convolve(const dsp_filter_t *cfg) {
  /* Full (linear) convolution length = N + M - 1 */
  int out_len = cfg->input_len + cfg->kernel_len - 1;

  for (int n = 0; n < out_len; n++) {
    double acc = 0.0;

    /* Slide the kernel over the input; accumulate element-wise products.
     * Samples outside [0, input_len) are implicitly zero (zero-padding). */
    for (int k = 0; k < cfg->kernel_len; k++) {
      int idx = n - k; /* corresponding input index for this tap */
      if (idx >= 0 && idx < cfg->input_len) {
        acc += cfg->input[idx] * cfg->kernel[k];
      }
    }
    cfg->output[n] = acc;
  }
}
