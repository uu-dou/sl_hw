/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the declaration of subband analyse and subband compose 
  *
  **************************************************************************/

#ifndef _DIOS_SSP_SHARE_SUBBAND_H_
#define _DIOS_SSP_SHARE_SUBBAND_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dios_ssp_share_rfft.h"

typedef struct {
	int frm_len;
	int Ppf_tap;
	int Ppf_decm;
	int *p_in;
	int *p_h0;
	float scale;
	float *ana_xin;       // time domain data input for analyze
	xcomplex *ana_cxout;  // frequency domain complex output for analyze
	float *ana_xout;      // time domain output for analyze

	float *comp_in;       // time domain data input for compose
	float *comp_out;      // data output for compose
	float* lpf_coef;      // window function
	void *rfft_param;
	float *fftout_buffer;
	float *fftin_buffer;
}objSubBand;

objSubBand* dios_ssp_share_subband_init(int frm_len);
int dios_ssp_share_subband_reset(objSubBand* ptr);
int dios_ssp_share_subband_analyse(objSubBand* ptr, float* in_buf, xcomplex* out_buf);
int dios_ssp_share_subband_compose(objSubBand* ptr, xcomplex* in_buf, float* out_buf);
int dios_ssp_share_subband_uninit(objSubBand* ptr);

#endif /* _DIOS_SSP_SHARE_SUBBAND_H_ */

