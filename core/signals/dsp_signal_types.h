/**
 * @file      dsp_signal_types.h
 * @brief     Brief description of this module
 * @author    Karim Yasser
 * @date      2026-05-09
 * @copyright Copyright (c) 2026 Company Name
 */

#ifndef DSP_SIGNAL_TYPES_H
#define DSP_SIGNAL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Macros --------------------------------------------------------------------*/
/* Enums ---------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Structs -------------------------------------------------------------------*/
typedef struct dsp_FTsignal_s {
  double *signal;
  double *re;
  double *im;
  int size;
} dsp_FTsignal_t;

typedef struct dsp_cartPolar_s {
  double *re;
  double *im;
  double *mag;
  double *phase;
  int size;
} dsp_cartPolar_t;

/* Global Variables
   ----------------------------------------------------------*/
/* Function Prototypes
   -------------------------------------------------------*/

/**
 * @brief  Brief description of the function
 * @param  param_name Description of parameter
 * @return Description of return value
 */
void function_name(void);

#ifdef __cplusplus
}
#endif

#endif /* DSP_SIGNAL_TYPES_H */