/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the declaration of aec api. 
  *
  **************************************************************************/

#ifndef _DIOS_SSP_AEC_API_H_
#define _DIOS_SSP_AEC_API_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dios_ssp_aec_firfilter.h"
#include "dios_ssp_share_subband.h"
#include "dios_ssp_share_complex_defs.h"

void* dios_ssp_aec_init_api(int mic_num, int ref_num, int frm_len, int delay);
int dios_ssp_aec_reset_api(void* ptr);
int dios_ssp_aec_process_api(void* ptr, float* io_buf, float* ref_buf, int* dt_st);
int dios_ssp_aec_uninit_api(void* ptr);

#endif /* _DIOS_SSP_AEC_API_H_ */

