/***************************************************************************
  * 
  * Homework for chapter 5 -- Acoustic Echo Cancellation
  *
  * In the main function, we have finished data preparation for you.
  * Input signal and reference signal, input.pcm & ref.pcm, are read,
  * frame by frame. Then AEC is applied.
  *
  * The format of input/output audio is pcm ".raw". You can use Audition 
  * or Cooledit to see the waveform or spectrogram of pcm audio files.
  * 
  **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "dios_ssp_api.h"

const int array_frm_len = 128;

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        printf("usage: %s input.raw ref.raw aec_output.raw\r\n", argv[0]);
        return -1;
    }

    int ii;
    
    FILE *fpin = NULL;
    FILE *fpref = NULL;
    FILE *fpout = NULL;
    short *ptr_input_data = (short*)calloc(array_frm_len, sizeof(short));
    short *ptr_output_data = (short*)calloc(array_frm_len, sizeof(short));
    short *ptr_temp = (short*)calloc(array_frm_len, sizeof(short));
    short *ptr_ref_data = (short*)calloc(array_frm_len, sizeof(short));
   
    fpin = fopen(argv[1], "rb");
    fseek(fpin, 0, SEEK_END);
    long file_len = ftell(fpin);
    file_len /= 2;
    rewind(fpin);
    long frame_num = (file_len - array_frm_len * 128) / array_frm_len;
    int sample_res = (file_len - array_frm_len * 128) % array_frm_len;
    printf("framenum: %ld and %d samples.\r\n", frame_num, sample_res);
    fpref = fopen(argv[2], "rb");
    fpout = fopen(argv[3], "wb");

    // TDE Begin
    int delay = 0;
    short* ptr_input_for_tde = (short*)calloc(array_frm_len * 128, sizeof(short));
    short* ptr_ref_for_tde = (short*)calloc(array_frm_len * 128, sizeof(short));
    fread(ptr_input_for_tde, sizeof(short), array_frm_len * 128, fpin);
    fread(ptr_ref_for_tde, sizeof(short), array_frm_len * 128, fpref);
    delay = dios_ssp_tde(ptr_input_for_tde, ptr_ref_for_tde, array_frm_len * 128);
    // TDE End

    // AEC Begin
    void* st;
    st = dios_ssp_init_api(delay);
    dios_ssp_reset_api(st);
    for(ii = 0; ii < frame_num; ii++)
    {
        // prepare input signal
        fread(ptr_temp, sizeof(short), array_frm_len, fpin);
        memcpy(ptr_input_data, ptr_temp, sizeof(short) * array_frm_len);
        // prepare ref signal
        fread(ptr_temp, sizeof(short), array_frm_len, fpref);
        memcpy(ptr_ref_data, ptr_temp, sizeof(short) * array_frm_len);
        // AEC
        dios_ssp_process_api(st, ptr_input_data, ptr_ref_data, ptr_output_data);
        // save output file
        fwrite(ptr_output_data, sizeof(short), array_frm_len, fpout);
    }
    fwrite(ptr_output_data, sizeof(short), sample_res, fpout);
    dios_ssp_uninit_api(st);
    // AEC End

    fclose(fpin);
    fclose(fpref);
    fclose(fpout);
    free(ptr_temp);
    free(ptr_input_data);
    free(ptr_output_data);
    free(ptr_ref_data);
    free(ptr_input_for_tde);
    free(ptr_ref_for_tde);
    printf("process finished.\r\n");
    return 0;
}

