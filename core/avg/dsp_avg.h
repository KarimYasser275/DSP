/**
 * @file      dsp_avg.h
 * @brief     Brief description of this module
 * @author    Karim Yasser
 * @date      2026-05-18
 * @copyright Copyright (c) 2026 Company Name
 */

#ifndef DSP_AVG_H
#define DSP_AVG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Macros --------------------------------------------------------------------*/
/* Enums ---------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
typedef struct mvAvg_s {
  int32_t sum;
  int16_t *buff;
  uint16_t average;
  uint8_t size;
  uint8_t index;
} mvAvg_t;

/**
 * @brief  Enhanced moving average using power-of-2 buffer size.
 *         Tracks sample count for correct average during warm-up.
 * @note   size MUST be a power of 2 (e.g. 4, 8, 16, 32 ...)
 */
typedef struct mvAvgEnhanced_s {
  int32_t sum;      /**< Running sum of samples in the buffer          */
  int16_t *buff;    /**< Circular buffer (caller-allocated, size items) */
  int32_t average;  /**< Current average (higher precision than basic)  */
  uint16_t size;    /**< Buffer length, MUST be a power of 2           */
  uint16_t mask;    /**< size - 1, used for fast bitmask modulo        */
  uint16_t index;   /**< Next write position in the buffer             */
  uint16_t count;   /**< Samples added so far (saturates at size)      */
} mvAvgEnhanced_t;

/**
 * @brief  Recursive (exponential) moving average — no buffer needed.
 *         Uses fixed-point arithmetic: alpha = 1 / (1 << shift).
 *         Larger shift = heavier smoothing.
 */
typedef struct mvAvgRecursive_s {
  int32_t average;  /**< Smoothed output in Q16 fixed-point            */
  uint8_t shift;    /**< Smoothing factor: alpha = 1 / (1 << shift)    */
  bool initialized; /**< false until first sample is received          */
} mvAvgRecursive_t;
/* Structs -------------------------------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Function Prototypes -------------------------------------------------------*/

/**
 * @brief  Compute basic moving average (simple circular buffer).
 * @param  avg   Pointer to a zero-initialised mvAvg_t whose buff and size
 *               have been set by the caller.
 * @param  input New sample to add.
 */
void dsp_movingAvgCalc(mvAvg_t *avg, int16_t input);

/**
 * @brief  Enhanced moving average — power-of-2 bitmask indexing and correct
 *         average during the warm-up period (before the buffer is full).
 * @param  avg   Pointer to a zero-initialised mvAvgEnhanced_t whose buff,
 *               size, and mask have been set by the caller.
 * @param  input New sample to add.
 */
void dsp_movingAvgEnhancedCalc(mvAvgEnhanced_t *avg, int16_t input);

/**
 * @brief  Recursive (exponential) moving average — IIR low-pass filter.
 *         y[n] = y[n-1] + (x[n] - y[n-1]) >> shift
 *         No buffer required; only a single int32_t state is kept.
 * @param  avg   Pointer to a mvAvgRecursive_t whose shift has been set.
 * @param  input New sample to add.
 */
void dsp_movingAvgRecursiveCalc(mvAvgRecursive_t *avg, int16_t input);
#ifdef __cplusplus
}
#endif

#endif /* DSP_AVG_H */