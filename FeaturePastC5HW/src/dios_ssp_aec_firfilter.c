/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the realization of linear filter (fir filter).
  * You are going to finish the linear echo cancellation part, i.e.
  * nlms_complex() function, in line 216.
  *
  **************************************************************************/

#include "dios_ssp_aec_firfilter.h"

/**
 * @brief estimate echo and calculate residual
 *
 * Estimated echo signals are obtained by convolution of ref signal 'srv->stack_sigIn_adf'
 * and fir filter 'srv->fir_coef' && 'srv->adf_coef'. Error signals are calculated. The fir
 * filter order is stored in 'srv->num_main_subband_adf'.
 */
void dios_ssp_aec_residual(objFirFilter *srv)
{
	int ch;
	int i_ref;
	int i;
	for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
    {
		for (ch = AEC_LOW_CHAN; ch < AEC_HIGH_CHAN; ch++)
		{
			/* get reference vector for fir filter */
			complex_data_push(srv->num_main_subband_adf[ch] + 1, srv->sig_spk_ref[i_ref][ch], srv->stack_sigIn_adf[i_ref][ch]);
			
			/* get echo signal: conv: y = conj(h) * x */
			srv->est_ref_fir[i_ref][ch] = complex_conv(srv->num_main_subband_adf[ch], srv->fir_coef[i_ref][ch],
					srv->stack_sigIn_adf[i_ref][ch]);
			srv->est_ref_adf[i_ref][ch] = complex_conv(srv->num_main_subband_adf[ch], srv->adf_coef[i_ref][ch], 
					srv->stack_sigIn_adf[i_ref][ch]);
			
			/* get power of reference vector */
			srv->power_in_ntaps_smooth[i_ref][ch] = 0.0f;
			for (i = 0; i < srv->num_main_subband_adf[ch]; i++)
			{
			    srv->power_in_ntaps_smooth[i_ref][ch] += complex_abs2(srv->stack_sigIn_adf[i_ref][ch][i]);
			}
		}
    }
	for (ch = AEC_LOW_CHAN; ch < AEC_HIGH_CHAN; ch++)
	{
		/* get total error signal for each reference signal */
        srv->err_fir[ch] = complex_sub(srv->sig_mic_rec[ch], srv->est_ref_fir[0][ch]);
        srv->err_adf[ch] = complex_sub(srv->sig_mic_rec[ch], srv->est_ref_adf[0][ch]);
        for (i_ref = 1; i_ref < srv->ref_num; i_ref++)
        {
            srv->err_fir[ch] = complex_sub(srv->err_fir[ch], srv->est_ref_fir[i_ref][ch]);
            srv->err_adf[ch] = complex_sub(srv->err_adf[ch], srv->est_ref_adf[i_ref][ch]);
        }
		/* estimate the smooth value for err_fir, err_adf and sig_mic_rec signal */
		srv->mse_adpt[ch] = (srv->lambda[ch] * srv->mse_adpt[ch]) + (1 - srv->lambda[ch]) * complex_abs2(srv->err_adf[ch]);
		srv->mse_main[ch] = (srv->lambda[ch] * srv->mse_main[ch]) + (1 - srv->lambda[ch]) * complex_abs2(srv->err_fir[ch]);
		srv->mse_mic_in[ch] = (srv->lambda[ch] * srv->mse_mic_in[ch]) + (1 - srv->lambda[ch]) * complex_abs2(srv->sig_mic_rec[ch]);
	}
}

/**
 * @brief filter convergence detection
 *
 */
void dios_ssp_aec_firfilter_detect(objFirFilter *srv)
{
	int ch, i_ref, i;
	for (ch = AEC_LOW_CHAN; ch < AEC_HIGH_CHAN; ch++)
    {
		if (srv->mse_adpt[ch] > srv->mse_mic_in[ch] * MSE_RATIO_OUT_IN)
		{
			for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
			{			 
				for (i = 0; i < srv->num_main_subband_adf[ch]; i++)
				{
					srv->adf_coef[i_ref][ch][i].r = 0.0;
					srv->adf_coef[i_ref][ch][i].i = 0.0;
				}
			}
			srv->mse_mic_in[ch] = 0.0;
			srv->mse_adpt[ch] = 0.0;
			srv->mse_main[ch] = 0.0;
		}
		else if ((srv->mse_mic_in[ch] > srv->mse_adpt[ch] * MSE_RATIO_OUT_IN)
				&& (srv->mse_adpt[ch] < FILTER_COPY_FAC * srv->mse_main[ch]))
		{
			for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
			{				
				for (i = 0; i < srv->num_main_subband_adf[ch]; i++)
				{
					srv->fir_coef[i_ref][ch][i] = srv->adf_coef[i_ref][ch][i];
				}
			}
			srv->mse_mic_in[ch] = 0.0;
			srv->mse_adpt[ch] = 0.0;
			srv->mse_main[ch] = 0.0;
		}

		if (srv->mse_main[ch] > srv->mse_mic_in[ch] * MSE_RATIO_OUT_IN)
		{
			for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
			{				
				for (i = 0; i < srv->num_main_subband_adf[ch]; i++)
				{
					srv->fir_coef[i_ref][ch][i].r = 0.0;
					srv->fir_coef[i_ref][ch][i].i = 0.0;
				}
			}
			srv->mse_main[ch] = 0.0;
			srv->mse_adpt[ch] = 0.0;
			srv->mse_mic_in[ch] = 0.0;
		}
		else if ((srv->mse_mic_in[ch] > srv->mse_main[ch] * MSE_RATIO_OUT_IN)
				&& (srv->mse_main[ch] < FILTER_COPY_FAC * srv->mse_adpt[ch]))
		{
			for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
			{
				for (i = 0; i<srv->num_main_subband_adf[ch]; i++)
				{
					srv->adf_coef[i_ref][ch][i] = srv->fir_coef[i_ref][ch][i];
				}
			}
			srv->mse_mic_in[ch] = 0.0;
			srv->mse_adpt[ch] = 0.0;
			srv->mse_main[ch] = 0.0;
			srv->err_adf[ch] = srv->err_fir[ch];
		}
	}
}

/**
 * @brief  get linear echo cancellation output and estimated echo.
 * 
 * @param output_buf    echo cancellation output
 * @param est_echo      estimated echo signal
 */
void dios_ssp_estecho_output(objFirFilter *srv, xcomplex* output_buf, xcomplex* est_echo)
{
	int ch;
	int i_ref;

	for(ch = AEC_LOW_CHAN; ch < AEC_HIGH_CHAN; ch++)
	{
		srv->energy_err_fir[ch] = complex_abs2(srv->err_fir[ch]);
		srv->energy_err_adf[ch] = complex_abs2(srv->err_adf[ch]);
		
		if (srv->energy_err_fir[ch] >= srv->energy_err_adf[ch])
		{   
			//adf_coef
			output_buf[ch] = srv->err_adf[ch];
			est_echo[ch] = srv->est_ref_adf[0][ch];
			for (i_ref = 1; i_ref < srv->ref_num; i_ref++)
			{
				est_echo[ch] = complex_add(est_echo[ch], srv->est_ref_adf[i_ref][ch]);
			}
		}
		else
		{ 
			//fir_coef
			output_buf[ch] = srv->err_fir[ch];
			est_echo[ch] = srv->est_ref_fir[0][ch];
			for (i_ref = 1; i_ref < srv->ref_num; i_ref++)
			{
				est_echo[ch] = complex_add(est_echo[ch], srv->est_ref_fir[i_ref][ch]);
			}
		}
	}
}

int aec_channel_to_band(int **band_table, int ch)
{
    int index;
    if (ch >= band_table[0][0] && ch <= band_table[0][1])
    {
        index = 0;
    }
    else if (ch >= band_table[1][0] && ch <= band_table[1][1])
    {
	    index = 1;
    }
    else if (ch >= band_table[2][0] && ch <= band_table[2][1])
    {
	    index = 2;
    }
    else
    {
	    index = 3;
    }

    return index;
}

/**
 * @brief  NLMS for linear echo cancellation.
 * 
 * You have to finish this function nlms_complex() by yourself.
 * It is called by dios_ssp_aec_firfilter_process() function.
 * The filter order is stored in srv->num_main_subband_adf[].
 * Reference signal is stored in srv->stack_sigIn_adf[][][].
 * Error signal is stored in srv->err_adf[].
 * Stepsize is stored in srv->weight[ch].
 * 
 * You may want to use complex operations, which are defined
 * in dios_ssp_share_complex_defs.{c, h}. Find what you want 
 * here.
 *
 * Only coefficients of filter 'srv->adf_coef' need to be updated. 
 * You don't need to deal with 'srv->fir_coef' here.
 *
 * @param ch     channel index
 * @param srv    pointer to objFirFilter struct
 * @param i_ref  reference index. For single reference scenario, 
 *               it equals to 0
 */
void nlms_complex(int ch, objFirFilter *srv, int i_ref)
{
    // please finish your code here
    
    // end NLMS
}

/**
 * @brief fir filter init
 *
 */
objFirFilter* dios_ssp_aec_firfilter_init(int ref_num)
{
	int i;
	int i_ref;
	int ret = 0;
    objFirFilter* srv = NULL;
    srv = (objFirFilter*)calloc(1, sizeof(objFirFilter));
	
	srv->ref_num = ref_num;
    srv->myu = 0.5f;
    srv->beta = 1e-008f;
    srv->fir_coef = (xcomplex ***)calloc(srv->ref_num, sizeof(xcomplex**));
    srv->adf_coef = (xcomplex ***)calloc(srv->ref_num, sizeof(xcomplex**));
    srv->stack_sigIn_adf = (xcomplex ***)calloc(srv->ref_num, sizeof(xcomplex**));
    srv->err_adf = (xcomplex *)calloc(AEC_SUBBAND_NUM, sizeof(xcomplex));
    srv->err_fir = (xcomplex *)calloc(AEC_SUBBAND_NUM, sizeof(xcomplex));
    srv->est_ref_adf = (xcomplex **)calloc(srv->ref_num, sizeof(xcomplex*));
    srv->est_ref_fir = (xcomplex **)calloc(srv->ref_num, sizeof(xcomplex*));

    srv->mse_main = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
    srv->mse_adpt = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
    srv->mse_mic_in = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
    srv->power_in_ntaps_smooth = (float **)calloc(srv->ref_num, sizeof(float*));
    srv->mic_rec_psd = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
	srv->energy_err_fir = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
	srv->energy_err_adf = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
	srv->ref_psd = (float **)calloc(srv->ref_num, sizeof(float*));
    srv->power_mic_send_smooth = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
    srv->power_echo_rtn_smooth = (float **)calloc(srv->ref_num, sizeof(float*));
    srv->mic_rec_part_band_energy = (float *)calloc(ERL_BAND_NUM, sizeof(float));

    srv->echo_return_band_energy = (float *)calloc(ERL_BAND_NUM, sizeof(float));
    srv->mic_send_part_band_energy = (float *)calloc(ERL_BAND_NUM, sizeof(float));
    srv->mic_peak = (float **)calloc(srv->ref_num, sizeof(float*));
    srv->erl_ratio = (float **)calloc(srv->ref_num, sizeof(float*));
    srv->power_echo_rtn_fir = (float **)calloc(srv->ref_num, sizeof(float*));
    srv->power_echo_rtn_adpt = (float **)calloc(srv->ref_num, sizeof(float*));
    for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
    {
		srv->ref_psd[i_ref] = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
		srv->power_in_ntaps_smooth[i_ref] = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
		srv->adf_coef[i_ref] = (xcomplex **)calloc(AEC_SUBBAND_NUM, sizeof(xcomplex*));
		srv->fir_coef[i_ref] = (xcomplex **)calloc(AEC_SUBBAND_NUM, sizeof(xcomplex*));
		srv->stack_sigIn_adf[i_ref] = (xcomplex **)calloc(AEC_SUBBAND_NUM, sizeof(xcomplex*));
		srv->power_echo_rtn_fir[i_ref] = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
		srv->power_echo_rtn_adpt[i_ref] = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
		for (i = 0; i < AEC_SUBBAND_NUM; i++)
		{
			srv->adf_coef[i_ref][i] = (xcomplex *)calloc((NUM_MAX_BAND + 1), sizeof(xcomplex));
			srv->fir_coef[i_ref][i] = (xcomplex *)calloc((NUM_MAX_BAND + 1), sizeof(xcomplex));
			srv->stack_sigIn_adf[i_ref][i] = (xcomplex *)calloc((NUM_MAX_BAND + 1), sizeof(xcomplex));
		}

		srv->est_ref_adf[i_ref] = (xcomplex *)calloc(AEC_SUBBAND_NUM, sizeof(xcomplex));
		srv->est_ref_fir[i_ref] = (xcomplex *)calloc(AEC_SUBBAND_NUM, sizeof(xcomplex));
		srv->power_echo_rtn_smooth[i_ref] = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
    }

    srv->num_main_subband_adf = (int *)calloc(AEC_SUBBAND_NUM, sizeof(int));
    srv->lambda = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
    srv->weight = (float *)calloc(AEC_SUBBAND_NUM, sizeof(float));
    
	for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
    {
		srv->mic_peak[i_ref] = (float *)calloc(ERL_BAND_NUM, sizeof(float));
		srv->erl_ratio[i_ref] = (float *)calloc(ERL_BAND_NUM, sizeof(float));
    }

	ret = dios_ssp_aec_firfilter_reset(srv);
	if(0 != ret)
	{
		return NULL;
	}
	
    return srv;
}

/**
 * @brief fir filter reset
 *
 */
int dios_ssp_aec_firfilter_reset(objFirFilter* srv)
{
	int i;
	int n;
	int i_ref;
	int ret = 0;

    if (NULL == srv) 
    {
		return ERR_AEC;
    }

	for (i = 0; i < AEC_SUBBAND_NUM; i++)
	{
		srv->weight[i] = srv->myu;
		if (i < AEC_MID_CHAN + 1)
		{
			srv->lambda[i] = ALPHA_MSE_FILT_COPY_LOW;
			srv->num_main_subband_adf[i] = NTAPS_LOW_BAND;
		}
		else
		{
			srv->lambda[i] = ALPHA_MSE_FILT_COPY_HIGH;
			srv->num_main_subband_adf[i] = NTAPS_HIGH_BAND;
		}
	}

	for (i = 0; i < AEC_SUBBAND_NUM; i++)
	{
		srv->err_fir[i].r = 0.0;
		srv->err_fir[i].i = 0.0;
		srv->err_adf[i].r = 0.0;
		srv->err_adf[i].i = 0.0;

		/* smoothed vector */
		srv->power_mic_send_smooth[i] = 0.0f;
		srv->mic_rec_psd[i] = 0.0f;

		srv->energy_err_fir[i] = 0.0f;
		srv->energy_err_adf[i] = 0.0f;

		for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
		{
			srv->ref_psd[i_ref][i] = 0.0f;
			srv->power_echo_rtn_smooth[i_ref][i] = 0.0f;
			srv->power_in_ntaps_smooth[i_ref][i] = 0.0f;
		}

		for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
		{
			for (n = 0; n < NUM_MAX_BAND + 1; n++)
			{
				srv->adf_coef[i_ref][i][n].r = 0.0f;
				srv->adf_coef[i_ref][i][n].i = 0.0f;
				srv->fir_coef[i_ref][i][n].r = 0.0f;
				srv->fir_coef[i_ref][i][n].i = 0.0f;
				srv->stack_sigIn_adf[i_ref][i][n].r = 0.0f;
				srv->stack_sigIn_adf[i_ref][i][n].i = 0.0f;
			}
		}
		srv->mse_adpt[i] = 0.0f;
		srv->mse_main[i] = 0.0f;
		srv->mse_mic_in[i] = 0.0f;
	}

	for (i = 0; i < ERL_BAND_NUM; i++)
	{
		srv->mic_rec_part_band_energy[i] = 0.0f;
		srv->mic_send_part_band_energy[i] = 0.0f;
		srv->echo_return_band_energy[i] = 0.0f;
		for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
		{
			srv->mic_peak[i_ref][i] = 0.0f;
			srv->erl_ratio[i_ref][i] = 4.0f;
		}
	}

	srv->adjust_flag = 0;
	return 0;
}

/**
 * @brief fir filter process
 *
 * @param srv         pointer of objFirFilter struct
 * @param output_buf  linear echo cancellation output
 * @param est_echo    estimated echo signal
 *
 */
int dios_ssp_aec_firfilter_process(objFirFilter* srv, xcomplex* output_buf, xcomplex* est_echo)
{
    int ch;
	int i_ref;
    float update_thr;
	float update_thr_final;

	if (NULL == srv)
	{
		return ERR_AEC;
	}

	/* get filter update threshold */
    if (srv->dt_status[0] == DOUBLE_TALK_STATUS)
    { 
		update_thr = FILTER_UPDATE_FAC_DT;
    }
    else
    {
	    update_thr = FILTER_UPDATE_FAC_NON_DT;
    }
	update_thr_final = update_thr;

	/* estimate echo and calculate residual */
	dios_ssp_aec_residual(srv);

	/* filter convergence detect */
	dios_ssp_aec_firfilter_detect(srv);

	/* filter update */
	for (ch = AEC_LOW_CHAN; ch < AEC_HIGH_CHAN; ch++)
    {
		if (AEC_SAMPLE_RATE == 16000)
		{
			if (ch >= FILTER_UPDATE_FAC_BIN_THR1)
			{
				update_thr_final = update_thr * FILTER_UPDATE_FAC_PARA1;
			}
			else if ((ch >= FILTER_UPDATE_FAC_BIN_THR2)&& (ch < FILTER_UPDATE_FAC_BIN_THR1))
			{
				update_thr_final = update_thr * FILTER_UPDATE_FAC_PARA2;
			}
		}
		
		for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
		{
			// achieve filter update control variable
			int iBand = aec_channel_to_band(srv->band_table, ch);

			if (srv->power_in_ntaps_smooth[i_ref][ch] * srv->erl_ratio[i_ref][iBand] > update_thr_final * SPK_MIN_NOISE)
			{
				srv->adjust_flag = 1;
			}
			else
			{
				srv->adjust_flag = 0;
			}

			if (srv->adjust_flag == 1) 
			{
				nlms_complex(ch, srv, i_ref);
			}
		}
    }

	/* get linear echo cancellation output */
    for (ch = 0; ch < AEC_LOW_CHAN; ch++)
    {
		output_buf[ch].r = 0.0f;
		output_buf[ch].i = 0.0f;
    }
    for (ch = AEC_HIGH_CHAN; ch < AEC_SUBBAND_NUM; ch++)
    {
		output_buf[ch].r = 0.0f;
		output_buf[ch].i = 0.0f;
    }
	/* get linear echo cancellation output */
	dios_ssp_estecho_output(srv, output_buf, est_echo);
    return 0;
}

/**
 * @brief fir filter uninit
 *
 */
int dios_ssp_aec_firfilter_uninit(objFirFilter* srv)
{
	int i;
	int i_ref;
    if (NULL == srv) 
    {
		return ERR_AEC;
    }
    for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
    {
		for (i = 0; i < AEC_SUBBAND_NUM; i++)
		{
			free(srv->adf_coef[i_ref][i]);
			free(srv->fir_coef[i_ref][i]);
			free(srv->stack_sigIn_adf[i_ref][i]);
		}
		
		free(srv->adf_coef[i_ref]);
		free(srv->fir_coef[i_ref]);
		free(srv->stack_sigIn_adf[i_ref]);
	
		free(srv->power_in_ntaps_smooth[i_ref]);
		free(srv->est_ref_adf[i_ref]);
		free(srv->est_ref_fir[i_ref]);
		free(srv->ref_psd[i_ref]);
		free(srv->power_echo_rtn_smooth[i_ref]);
		free(srv->power_echo_rtn_fir[i_ref]);
		free(srv->power_echo_rtn_adpt[i_ref]);
    }
    free(srv->power_echo_rtn_fir);
    free(srv->power_echo_rtn_adpt);
    free(srv->fir_coef);
    free(srv->adf_coef);
    free(srv->stack_sigIn_adf);
    free(srv->err_adf);
    free(srv->err_fir);
    free(srv->est_ref_adf);
    free(srv->est_ref_fir);

    free(srv->mse_main);
    free(srv->mse_adpt);
    free(srv->mse_mic_in);
    free(srv->power_in_ntaps_smooth);
    free(srv->mic_rec_psd);
	free(srv->energy_err_fir);
	free(srv->energy_err_adf);
    free(srv->power_mic_send_smooth);
	free(srv->ref_psd);
    free(srv->power_echo_rtn_smooth);

    free(srv->echo_return_band_energy);
    free(srv->mic_rec_part_band_energy);
    free(srv->mic_send_part_band_energy);

    for (i_ref = 0; i_ref < srv->ref_num; i_ref++)
    {
		free(srv->mic_peak[i_ref]);
		free(srv->erl_ratio[i_ref]);
    }
    free(srv->mic_peak);
    free(srv->erl_ratio);
    free(srv->num_main_subband_adf);
    free(srv->lambda);
    free(srv->weight);
    free(srv);
    return 0;
}

