/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the declaration of linear filter (fir filter).
  *
  **************************************************************************/

#ifndef _DIOS_SSP_AEC_FIRFILTER_H_
#define _DIOS_SSP_AEC_FIRFILTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dios_ssp_aec_macros.h"
#include "dios_ssp_share_complex_defs.h"

/* fir filter struct define */
typedef struct {
	int ref_num;
	int far_end_talk_holdtime;
	float myu;
	float beta;
	xcomplex** sig_spk_ref;
	xcomplex* sig_mic_rec; // data input for filter x, y
	float* err_out;        // filter result error output
	int* num_main_subband_adf;
	float* lambda;
	float* weight;
	xcomplex*** fir_coef;
	xcomplex*** adf_coef;
	xcomplex*** stack_sigIn_adf;
	xcomplex* err_adf;
	xcomplex* err_fir;
	xcomplex** est_ref_adf;
	xcomplex** est_ref_fir;
	float* mse_main;  //err_fir mse
	float* mse_adpt;  //err_adf mse
	float* mse_mic_in; // mse of mic input singal
	float** power_in_ntaps_smooth;
	float* mic_rec_psd;
	float *energy_err_fir;
	float *energy_err_adf;
	float* power_mic_send_smooth;
	float** power_echo_rtn_smooth;
	float** ref_psd;
	float** power_echo_rtn_fir;
	float** power_echo_rtn_adpt;
	// ERL estimate
	int** band_table;
	//float* spk_part_band_energy;
	float* echo_return_band_energy;
	float* mic_rec_part_band_energy;
	float* mic_send_part_band_energy;
	float** mic_peak;
	float** erl_ratio;
	int adjust_flag;
	// double talk detection
	int* dt_status;
}objFirFilter;

/**********************************************************************************
Function:      // dios_ssp_aec_firfilter_init
Description:   // load configure file and allocate memory
Input:         // ref_num: reference number
Output:        // none
Return:        // success: return dios speech signal process aec firfilter pointer
	              failure: return NULL
**********************************************************************************/
objFirFilter* dios_ssp_aec_firfilter_init(int ref_num);

/**********************************************************************************
Function:      // dios_ssp_aec_firfilter_reset
Description:   // reset dios speech signal process aec firfilter module
Input:         // ptr: dios speech signal process aec firfilter pointer
Output:        // none
Return:        // success: return 0, failure: return ERR_AEC
**********************************************************************************/
int dios_ssp_aec_firfilter_reset(objFirFilter* ptr);
	
/**********************************************************************************
Function:      // dios_ssp_aec_firfilter_process
Description:   // run dios speech signal process aec firfilter module by frames
Input:         // ptr: dios speech signal process aec firfilter pointer
Output:        // output_buf: error signal output
	              est_echo: estimated echo signal output
Return:        // success: return 0, failure: return ERR_AEC
**********************************************************************************/
int dios_ssp_aec_firfilter_process(objFirFilter* ptr, xcomplex* output_buf, xcomplex* est_echo);

/**********************************************************************************
Function:      // dios_ssp_aec_firfilter_uninit
Description:   // free dios speech signal process aec firfilter module
Input:         // srv: dios speech signal process aec firfilter pointer
Output:        // none
Return:        // success: return 0, failure: return ERR_AEC
**********************************************************************************/
int dios_ssp_aec_firfilter_uninit(objFirFilter* ptr);

#endif /* _DIOS_SSP_AEC_FIRFILTER_H_ */

