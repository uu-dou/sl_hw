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
    
    // please finish your code here...
    
    // end TDE
    
    return delay;
}

