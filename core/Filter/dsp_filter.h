/**
 * @file      dsp_filter.h
 * @brief     Low-pass and high-pass FIR filter interface
 * @author    Karim Yasser
 * @date      2026-05-23
 * @copyright Copyright (c) 2026 Company Name
 */

#ifndef DSP_FILTER_H
#define DSP_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Macros --------------------------------------------------------------------*/
#define DSP_Q15_SHIFT  15              /**< Fractional bits for Q15 format   */
#define DSP_Q15_SCALE  (1 << DSP_Q15_SHIFT) /**< Q15 scale factor (32768)    */
/* Enums ---------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/

/**
 * @brief  Configuration and data for a FIR low-pass / high-pass / band-pass filter.
 *
 * The caller must allocate all arrays before calling the filter function:
 *   - input       : input signal array           (input_len elements)
 *   - output      : output signal array           (input_len + kernel_len - 1)
 *   - kernel      : scratch space for the FIR tap  (kernel_len elements)
 */
typedef struct dsp_filter_s
{
    double *input;       /**< Pointer to the input signal array                */
    double *output;      /**< Pointer to the output signal array               */
    double *kernel;      /**< Pointer to the filter kernel (FIR taps) buffer   */
    int     input_len;   /**< Number of samples in the input signal            */
    int     kernel_len;  /**< Number of taps in the filter kernel (must be odd)*/
    double  fc;          /**< Normalised cutoff frequency (0.0 – 0.5)          */
    double  fc_high;     /**< Upper normalised cutoff for BPF (0.0 – 0.5)      */
} dsp_filter_t;

/**
 * @brief  State and coefficients for a 1st-order IIR filter.
 *
 * Difference equation:  y[n] = a0·x[n] + a1·x[n-1] - b1·y[n-1]
 *
 * The caller sets fc, fs, and is_hpf, then calls dsp_1stOrderFilterInit()
 * which computes a0, a1, b1 and zeroes the state variables.
 */
typedef struct dsp_1stOrderFilter_s
{
    double a0;     /**< Feedforward coefficient for x[n]              */
    double a1;     /**< Feedforward coefficient for x[n-1]            */
    double b1;     /**< Feedback coefficient for y[n-1]               */
    double x_prev; /**< Previous input  sample  x[n-1]               */
    double y_prev; /**< Previous output sample  y[n-1]               */
    double fc;     /**< Cutoff frequency in Hz                        */
    double fs;     /**< Sampling frequency in Hz                      */
    bool   is_hpf; /**< false = low-pass, true = high-pass            */
} dsp_1stOrderFilter_t;

/**
 * @brief  Fixed-point (Q15) state and coefficients for a 1st-order IIR filter.
 *
 * Same difference equation as the floating-point variant:
 *   y[n] = a0·x[n] + a1·x[n-1] - b1·y[n-1]
 *
 * All coefficients are stored as Q15 signed integers (divide by 32768 to get
 * the real value).  Multiplications are performed in 32-bit and shifted back
 * to 16-bit.  The caller sets fc, fs, and is_hpf, then calls
 * dsp_1stOrderFilterFPInit().
 */
typedef struct dsp_1stOrderFilterFP_s
{
    int16_t a0;     /**< Feedforward coefficient for x[n]   (Q15)       */
    int16_t a1;     /**< Feedforward coefficient for x[n-1] (Q15)       */
    int16_t b1;     /**< Feedback coefficient for y[n-1]    (Q15)       */
    int16_t x_prev; /**< Previous input  sample  x[n-1]                */
    int16_t y_prev; /**< Previous output sample  y[n-1]                */
    double  fc;     /**< Cutoff frequency in Hz  (used only by Init)    */
    double  fs;     /**< Sampling frequency in Hz (used only by Init)   */
    bool    is_hpf; /**< false = low-pass, true = high-pass             */
} dsp_1stOrderFilterFP_t;

/* Structs -------------------------------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Function Prototypes -------------------------------------------------------*/

/**
 * @brief  Apply a windowed-sinc FIR low-pass filter.
 *
 * Generates a sinc kernel with a Blackman window at the cutoff frequency
 * stored in cfg->fc, then convolves it with cfg->input.
 * Result is written to cfg->output (length = input_len + kernel_len - 1).
 *
 * @param  cfg  Pointer to a fully initialised dsp_filter_t.
 */
void dsp_LPF(dsp_filter_t *cfg);

/**
 * @brief  Apply a windowed-sinc FIR high-pass filter.
 *
 * Generates a low-pass sinc kernel with a Blackman window, then spectrally
 * inverts it to obtain a high-pass kernel.  The kernel is convolved with
 * cfg->input and the result is written to cfg->output.
 *
 * @param  cfg  Pointer to a fully initialised dsp_filter_t.
 */
void dsp_HPF(dsp_filter_t *cfg);

/**
 * @brief  Apply a windowed-sinc FIR band-pass filter.
 *
 * Builds two LPF kernels (at fc_high and fc), subtracts them to form a
 * band-pass kernel, then convolves it with cfg->input.
 * Uses the first kernel_len elements of cfg->output as scratch space
 * (overwritten by the convolution afterwards).
 *
 * @param  cfg  Pointer to a fully initialised dsp_filter_t with both
 *              fc (lower cutoff) and fc_high (upper cutoff) set.
 */
void dsp_BPF(dsp_filter_t *cfg);

/**
 * @brief  Initialise a 1st-order IIR filter.
 *
 * Computes the filter coefficients (a0, a1, b1) from the cutoff and
 * sampling frequencies stored in the struct and resets the internal state.
 * Must be called before the first call to dsp_1stOrderFilterCalc().
 *
 * @param  filt  Pointer to a dsp_1stOrderFilter_t whose fc, fs, and is_hpf
 *               fields have been set by the caller.
 */
void dsp_1stOrderFilterInit(dsp_1stOrderFilter_t *filt);

/**
 * @brief  Process one sample through a 1st-order IIR filter.
 *
 * Applies the difference equation y[n] = a0·x[n] + a1·x[n-1] - b1·y[n-1]
 * and updates the internal state.
 *
 * @param  filt   Pointer to an initialised dsp_1stOrderFilter_t.
 * @param  input  New input sample x[n].
 * @return Filtered output sample y[n].
 */
double dsp_1stOrderFilterCalc(dsp_1stOrderFilter_t *filt, double input);

/**
 * @brief  Initialise a 1st-order IIR filter in Q15 fixed-point.
 *
 * Computes floating-point coefficients from fc / fs, quantises them to Q15,
 * and resets the filter state.  Must be called before dsp_1stOrderFilterFPCalc().
 *
 * @param  filt  Pointer to a dsp_1stOrderFilterFP_t whose fc, fs, and is_hpf
 *               fields have been set by the caller.
 */
void dsp_1stOrderFilterFPInit(dsp_1stOrderFilterFP_t *filt);

/**
 * @brief  Process one sample through a Q15 fixed-point 1st-order IIR filter.
 *
 * Uses 32-bit multiply-accumulate internally, then shifts back to Q15.
 *
 * @param  filt   Pointer to an initialised dsp_1stOrderFilterFP_t.
 * @param  input  New input sample x[n] (plain int16_t, not Q-formatted).
 * @return Filtered output sample y[n].
 */
int16_t dsp_1stOrderFilterFPCalc(dsp_1stOrderFilterFP_t *filt, int16_t input);

#ifdef __cplusplus
}
#endif

#endif /* DSP_FILTER_H */