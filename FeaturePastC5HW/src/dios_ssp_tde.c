/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the realization of Time Delay Estimation api. 
  *
  **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dios_ssp_tde.h"
#include "dios_ssp_share_rfft.h"

/**
 * @brief tde process
 *
 * You are going to finish this function of TDE, dios_ssp_tde().
 * Obviously the time delay is within [0, inputdata_length).
 * Since we don't know the ground truth delay, we usually need 
 * a longer buffer for TDE. In this lab we use about 1s buffer
 * for TDE. You can apply GCC method to estimate the time delay
 * between input signal and reference signal. 
 *
 * @param inputdata          input data buffer.
 * @param refdata            reference data buffer.
 * @param inputdata_length   input data length (samples), also the reference data length.
 * @return                   estimated time delay (samples) between input signal and reference signal.
 */
int dios_ssp_tde(short* inputdata, short* refdata, long int inputdata_length)
{
    int delay = 0;
    printf ("inputdata_length:%d\n", inputdata_length);

    // please finish your code here...

    // 归一化
    float *near = (float *) malloc (2*inputdata_length * sizeof(float));
    float *far  = (float *) malloc (2*inputdata_length * sizeof(float));
    for (int i = 0; i < 2*inputdata_length; i++) {
        near[i] = (float)inputdata[i]/32768.f;
        far[i]  = (float)refdata[i]/32768.f;
    }

    int fft_size = inputdata_length;
    int subband_num = fft_size/2 + 1;
    
    void *rfft_handle = dios_ssp_share_rfft_init(fft_size);

    // fft for inputdata
    float *near_fft = (float *)malloc(sizeof(float) * fft_size);
    memset ((void *)near_fft, 0, sizeof(float) * fft_size);
    dios_ssp_share_rfft_process(rfft_handle, near, near_fft);
    xcomplex* x_near_fft = malloc (subband_num * sizeof(xcomplex));
	for (int i = 0; i < subband_num; i++)
    {
	    x_near_fft[i].r = near_fft[i];
    }
    x_near_fft[0].i = x_near_fft[subband_num - 1].i = 0;
    for (int i = 1; i < subband_num - 1; i++)
    {
	    x_near_fft[i].i = -near_fft[fft_size - i];
    }

    // fft for refdata and conj fft
    float *far_fft = (float *)malloc(sizeof(float) * fft_size);
    dios_ssp_share_rfft_process(rfft_handle, far, far_fft);
    xcomplex* x_far_fft_conj = (xcomplex*)malloc (subband_num * sizeof(xcomplex));
	for (int i = 0; i < subband_num; i++)
    {
	    x_far_fft_conj[i].r = far_fft[i];
    }
    x_far_fft_conj[0].i = x_far_fft_conj[subband_num - 1].i = 0;
    for (int i = 1; i < subband_num - 1; i++)
    {
	    x_far_fft_conj[i].i = far_fft[fft_size - i];
    }

    // x_near_fft.*x_far_fft_conj
    xcomplex* xcorrfft =  (xcomplex*)malloc (subband_num * sizeof(xcomplex));
    for (int i = 0; i < subband_num - 1; i++) {
        xcorrfft[i].r = x_near_fft[i].r * x_far_fft_conj[i].r - x_near_fft[i].i * x_far_fft_conj[i].i;
        xcorrfft[i].i = x_near_fft[i].i * x_far_fft_conj[i].r + x_near_fft[i].r * x_far_fft_conj[i].i;
    }

    float *input = (float *)malloc(fft_size * sizeof(float));
	input[0] = xcorrfft[0].r;
	input[subband_num] = xcorrfft[subband_num].r;
    for (int i = 1; i < subband_num; i++) {
		input[i] = xcorrfft[i].r;
		input[fft_size - i] = -xcorrfft[i].i;
	}
    float *xcorrfft_ifft = (float *)malloc(fft_size * sizeof(float));
    dios_ssp_share_irfft_process(rfft_handle, input, xcorrfft_ifft);

    float m = 0;
    for (int i = 0; i < inputdata_length; i++) {
        if (xcorrfft_ifft[i] > m) {
            delay = i;
            m = xcorrfft_ifft[i];
        }
    }

    printf ("delay:%d\n", delay);

    free (xcorrfft_ifft);
    free (input);
    free (xcorrfft);
    free (x_far_fft_conj);
    free (far_fft);
    free (x_near_fft);
    free (near_fft);
    free (far);
    free (near);
    // end TDE
    
    return delay;
}

