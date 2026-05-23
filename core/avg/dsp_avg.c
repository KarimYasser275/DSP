/**
 * @file      dsp_avg.c
 * @brief     Implementation of the module
 * @author    Karim Yasser
 * @date      2026-05-18
 * @copyright Copyright (c) 2026 Company Name
 */

/* Includes ------------------------------------------------------------------*/
#include "dsp_avg.h"

/* Private Macros ------------------------------------------------------------*/

/* Private Enums -------------------------------------------------------------*/

/* Private Typedefs ----------------------------------------------------------*/

/* Private Structs -----------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/

/* Public Function Implementations -------------------------------------------*/

/**
 * @brief  Compute basic moving average (simple circular buffer).
 */
void dsp_movingAvgCalc(mvAvg_t *avg, int16_t input) {
  avg->sum -= avg->buff[avg->index];
  avg->sum += input;
  avg->buff[avg->index] = input;
  avg->index += 1;
  if (avg->index >= avg->size) {
    avg->index = 0;
  }
  avg->average = avg->sum / avg->size;
}

/**
 * @brief  Enhanced moving average — bitmask indexing, warm-up aware.
 */
void dsp_movingAvgEnhancedCalc(mvAvgEnhanced_t *avg, int16_t input) {
  /* Subtract the oldest sample only after the buffer is full */
  if (avg->count >= avg->size) {
    avg->sum -= avg->buff[avg->index];
  }

  /* Insert new sample and advance index using bitmask (no branch/modulo) */
  avg->sum += input;
  avg->buff[avg->index] = input;
  avg->index = (avg->index + 1) & avg->mask;

  /* Track how many samples have been added (saturate at size) */
  if (avg->count < avg->size) {
    avg->count++;
  }

  /* Divide by actual sample count so early averages are correct */
  avg->average = avg->sum / avg->count;
}

/**
 * @brief  Recursive (exponential) moving average — IIR low-pass filter.
 *         Uses Q16 fixed-point: average stores (true_avg << 16).
 */
void dsp_movingAvgRecursiveCalc(mvAvgRecursive_t *avg, int16_t input) {
  if (!avg->initialized) {
    /* Seed the filter with the first sample (in Q16) */
    avg->average = (int32_t)input << 16;
    avg->initialized = true;
    return;
  }

  /* EMA: y[n] = y[n-1] + (x[n] - y[n-1]) * alpha
   * With alpha = 1 / (1 << shift), implemented as a right-shift.
   * input is promoted to Q16 before computing the error term. */
  int32_t input_q16 = (int32_t)input << 16;
  avg->average += (input_q16 - avg->average) >> avg->shift;
}

/* Private Function Implementations ------------------------------------------*/
