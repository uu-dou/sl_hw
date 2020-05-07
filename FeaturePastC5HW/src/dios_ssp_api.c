/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * Here is the realization of outer api. 
  *
  **************************************************************************/

#include "dios_ssp_api.h"

typedef struct {
	void* ptr_aec;
	float* ptr_mic_buf;
	float* ptr_ref_buf;
	float* ptr_data_buf;
    int cfg_frame_len;
    int cfg_mic_num;
    int cfg_ref_num;
    int dt_st;
} objAEC;

void* dios_ssp_init_api(int delay)
{
	void* ptr = NULL;
	ptr = (void*)malloc(sizeof(objAEC));
	objAEC* srv = (objAEC*)ptr;
    
    // params init
    srv->cfg_frame_len = 128;
    srv->cfg_mic_num = 1;
    srv->cfg_ref_num = 1;

    // aec init
    srv->ptr_aec = dios_ssp_aec_init_api(srv->cfg_mic_num, srv->cfg_ref_num, srv->cfg_frame_len, delay);

    // allocate memory
    srv->ptr_mic_buf = (float*)calloc(srv->cfg_mic_num * srv->cfg_frame_len, sizeof(float));
    srv->ptr_ref_buf = (float*)calloc(srv->cfg_ref_num * srv->cfg_frame_len, sizeof(float));
    srv->ptr_data_buf = (float*)calloc(srv->cfg_frame_len, sizeof(float));

    // variables init 
    srv->dt_st = 1;

	return ptr;
}

int dios_ssp_reset_api(void* ptr)
{
    if(ptr == NULL)
    {
        return -1;
    }

    objAEC* srv = (objAEC*)ptr;
    int ret;

    srv->dt_st = 1;

    ret = dios_ssp_aec_reset_api(srv->ptr_aec);
    if(ret != 0)
    {
        return -1;
    }

	return 0;
}

int dios_ssp_process_api(void* ptr, short* mic_buf, short* ref_buf, short* out_buf)
{
    if(ptr == NULL)
    {
        return -1;
    }

    objAEC* srv = (objAEC*)ptr;
    int ret;
    int i, j;
   
    // get input and reference data
    for(i = 0; i < srv->cfg_mic_num; i++)
    {
        for(j = 0; j < srv->cfg_frame_len; j++)
        {
            srv->ptr_mic_buf[i * srv->cfg_frame_len + j] = (float)(mic_buf[i * srv->cfg_frame_len + j]);
        }
    }
    for(i = 0; i < srv->cfg_ref_num; i++)
    {
        for(j = 0; j < srv->cfg_frame_len; j++)
        {
            srv->ptr_ref_buf[i * srv->cfg_frame_len + j] = (float)(ref_buf[i * srv->cfg_frame_len + j]);
        }
    }

    // aec process
    ret = dios_ssp_aec_process_api(srv->ptr_aec, srv->ptr_mic_buf, srv->ptr_ref_buf, &srv->dt_st);
    if(ret != 0)
    {
        return -1;
    }

    // save output
    memcpy(srv->ptr_data_buf, &srv->ptr_mic_buf[0], srv->cfg_frame_len * sizeof(float));
    for(j = 0; j < srv->cfg_frame_len; j++)
    {
        out_buf[j] = (short)(srv->ptr_data_buf[j]);
    }

    return 0;
}

int dios_ssp_uninit_api(void* ptr)
{
    if(ptr == NULL)
    {
        return -1;
    }

    objAEC* srv = (objAEC*)ptr;
    int ret;

    if(srv->ptr_mic_buf != NULL)
    {
        free(srv->ptr_mic_buf);
        srv->ptr_mic_buf = NULL;
    }
    if(srv->ptr_ref_buf != NULL)
    {
        free(srv->ptr_ref_buf);
        srv->ptr_ref_buf = NULL;
    }
    if(srv->ptr_data_buf != NULL)
    {
        free(srv->ptr_data_buf);
        srv->ptr_data_buf = NULL;
    }

    ret = dios_ssp_aec_uninit_api(srv->ptr_aec);
    if(ret != 0)
    {
        return -1;
    }

    free(srv);
	return 0;
}

