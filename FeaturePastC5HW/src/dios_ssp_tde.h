/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the declaration of Time Delay Estimation api. 
  *
  **************************************************************************/

#ifndef _DIOS_SSP_TDE_H_
#define _DIOS_SSP_TDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dios_ssp_tde(short* inputdata, short* refdata, long int inputdata_length);

#endif  /* _DIOS_SSP_TDE_H_ */

