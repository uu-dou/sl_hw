/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the declaration of outer api. 
  *
  **************************************************************************/

#ifndef _DIOS_SSP_API_H_
#define _DIOS_SSP_API_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dios_ssp_aec_api.h"

void* dios_ssp_init_api(int delay);
int dios_ssp_reset_api(void* ptr);
int dios_ssp_process_api(void* ptr, short* mic_buf, short* ref_buf, short* out_buf);
int dios_ssp_uninit_api(void* ptr);

#endif  /* _DIOS_SSP_API_H_ */

