/* Copyright (C) 2017 Beijing Didi Infinity Technology and Development Co.,Ltd.
All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef _DIOS_SSP_AEC_MACROS_H_
#define _DIOS_SSP_AEC_MACROS_H_

#define PI                                        (3.141592653589793f)

#define AEC_SAMPLE_RATE				              (16000)
#define AEC_FFT_LEN                               (256)
#define AEC_SUBBAND_NUM                           (AEC_FFT_LEN/2+1)
#define AEC_WIN_LEN                               (768)
#define AEC_LOW_CHAN                              (0) /* low band for process */
#define AEC_MID_CHAN                              (AEC_FFT_LEN/8-1) 
#define AEC_HIGH_CHAN                             (129)/* high band for process */
#define FAREND_TALK_CNT                           (20)

/* linear echo cancellation module */
#define NTAPS_LOW_BAND                            (10)          /* low band filter tap number */
#define NTAPS_HIGH_BAND                           (8)           /* hign band filter tap number */
#define NUM_MAX_BAND                              (NTAPS_LOW_BAND + 10)    /* max filter tap number */

/* smooth factor */
#define ALPHA_MSE_FILT_COPY_LOW                   (0.9735f)
#define ALPHA_MSE_FILT_COPY_HIGH                  (0.9608f)

/* firfilter submodule */
#define FILTER_UPDATE_FAC_NON_DT                  (5) 
#define FILTER_UPDATE_FAC_DT                      (FILTER_UPDATE_FAC_NON_DT * 40)
#define FILTER_UPDATE_FAC_BIN_THR1                (96)    //for 6kHz-8kHz
#define FILTER_UPDATE_FAC_PARA1                   (0.02f) //for 6kHz-8kHz
#define FILTER_UPDATE_FAC_BIN_THR2                (64)    //for 4kHz-6kHz
#define FILTER_UPDATE_FAC_PARA2                   (0.2f)  //for 4kHz-6kHz
#define MSE_RATIO_OUT_IN                          (8)
#define FILTER_COPY_FAC                           (0.5f)
#define NL_RUN_MIN_LEN                            (62) 
#define SPK_MAX_NOISE                             (100.0f)
#define SPK_MIN_NOISE                             (16.0f)
#define SPK_PART_MAX_NOISE                        (100.0f)
#define SPK_PART_MIN_NOISE                        (16.0f)
#define MIC_CHAN_MAX_NOISE                        (500.0f)
#define MIC_CHAN_MIN_NOISE                        (16.0f)

/* ERL estimate */
#define ERL_BOUND_H                               (32.0f)    // higher bound
#define ERL_BOUND_L                               (0.05f)    // lower bound
#define ERL_BAND_NUM                              (4)

/* doubletalk macros */
#define AEC_SINGLE_TALK_FRAMES                    (100)      // for AEC filter converge
#define	NEAREND_TALK_STATUS                       (2)
#define	SINGLE_TALK_STATUS                        (1) 
#define	DOUBLE_TALK_STATUS                        (0)

/* aec return values */
#define ERR_AEC                                   (-1)

#endif /* _DIOS_SSP_AEC_MACROS_H_ */

