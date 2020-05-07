/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the declaration of realfft 
  *
  **************************************************************************/

#ifndef _DIOS_SSP_SHARE_RFFT_H_
#define _DIOS_SSP_SHARE_RFFT_H_

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "dios_ssp_aec_macros.h"
#include "dios_ssp_share_complex_defs.h"

void *dios_ssp_share_rfft_init(int fft_len);    
int dios_ssp_share_rfft_process(void *rfft_handle, float *inbuffer, float *outbuffer);
int dios_ssp_share_irfft_process(void *rfft_handle, float *inbuffer, float *outbuffer);
int dios_ssp_share_rfft_uninit(void *rfft_handle);

#endif  /* _DIOS_SSP_SHARE_RFFT_H_ */

