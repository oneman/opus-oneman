/***********************************************************************
Copyright (c) 2006-2011, Skype Limited. All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, (subject to the limitations in the disclaimer below)
are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of Skype Limited, nor the names of specific
contributors, may be used to endorse or promote products derived from
this software without specific prior written permission.
NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "main.h"

static const int dec_delay_matrix[3][5] = {
/*SILK API 8  12  16  24  48 */
/* 8 */   {3, 0, 2, 0, 0},
/*12 */   {0, 8, 5, 7, 5},
/*16 */   {0, 0, 8, 5, 5}
};

/* Set decoder sampling rate */
opus_int silk_decoder_set_fs(
    silk_decoder_state          *psDec,                         /* I/O  Decoder state pointer                       */
    opus_int                    fs_kHz,                         /* I    Sampling frequency (kHz)                    */
    opus_int                    fs_API_Hz                       /* I    API Sampling frequency (Hz)                 */
)
{
    opus_int frame_length, ret = 0;

    silk_assert( fs_kHz == 8 || fs_kHz == 12 || fs_kHz == 16 );
    silk_assert( psDec->nb_subfr == MAX_NB_SUBFR || psDec->nb_subfr == MAX_NB_SUBFR/2 );

    /* New (sub)frame length */
    psDec->subfr_length = silk_SMULBB( SUB_FRAME_LENGTH_MS, fs_kHz );
    frame_length = silk_SMULBB( psDec->nb_subfr, psDec->subfr_length );

    /* Initialize resampler when switching internal or external sampling frequency */
    if( psDec->fs_kHz != fs_kHz || psDec->fs_API_hz != fs_API_Hz ) {
        /* Allocate worst case space for temporary upsampling, 8 to 48 kHz, so a factor 6 */
        opus_int16 temp_buf[ MAX_FRAME_LENGTH_MS * MAX_API_FS_KHZ ];
        silk_resampler_state_struct  temp_resampler_state;

        /* New delay value */
        psDec->delay = dec_delay_matrix[ rateID( silk_SMULBB( fs_kHz, 1000 ) ) ][ rateID( fs_API_Hz ) ];
        silk_assert( psDec->delay <= MAX_DECODER_DELAY );

        if( psDec->fs_kHz != fs_kHz && psDec->fs_kHz > 0 ) {
            /* Initialize resampler for temporary resampling of outBuf data to the new internal sampling rate */
            ret += silk_resampler_init( &temp_resampler_state, silk_SMULBB( psDec->fs_kHz, 1000 ), silk_SMULBB( fs_kHz, 1000 ) );

            /* Temporary resampling of outBuf data to the new internal sampling rate */
            silk_memcpy( temp_buf, psDec->outBuf, psDec->frame_length * sizeof( opus_int16 ) );
            ret += silk_resampler( &temp_resampler_state, psDec->outBuf, temp_buf, psDec->frame_length );
        }

        /* Initialize the resampler for dec_API.c preparing resampling from fs_kHz to API_fs_Hz */
        ret += silk_resampler_init( &psDec->resampler_state, silk_SMULBB( fs_kHz, 1000 ), fs_API_Hz );

        /* Correct resampler state by resampling buffered data from fs_kHz to API_fs_Hz */
        ret += silk_resampler( &psDec->resampler_state, temp_buf, psDec->outBuf, frame_length );

        psDec->fs_API_hz = fs_API_Hz;
    }

    if( psDec->fs_kHz != fs_kHz || frame_length != psDec->frame_length ) {
        if( fs_kHz == 8 ) {
            if( psDec->nb_subfr == MAX_NB_SUBFR ) {
                psDec->pitch_contour_iCDF = silk_pitch_contour_NB_iCDF;
            } else {
                psDec->pitch_contour_iCDF = silk_pitch_contour_10_ms_NB_iCDF;
            }
        } else {
            if( psDec->nb_subfr == MAX_NB_SUBFR ) {
                psDec->pitch_contour_iCDF = silk_pitch_contour_iCDF;
            } else {
                psDec->pitch_contour_iCDF = silk_pitch_contour_10_ms_iCDF;
            }
        }
        if( psDec->fs_kHz != fs_kHz ) {
            psDec->ltp_mem_length = silk_SMULBB( LTP_MEM_LENGTH_MS, fs_kHz );
            if( fs_kHz == 8 || fs_kHz == 12 ) {
                psDec->LPC_order = MIN_LPC_ORDER;
                psDec->psNLSF_CB = &silk_NLSF_CB_NB_MB;
            } else {
                psDec->LPC_order = MAX_LPC_ORDER;
                psDec->psNLSF_CB = &silk_NLSF_CB_WB;
            }
            if( fs_kHz == 16 ) {
                psDec->pitch_lag_low_bits_iCDF = silk_uniform8_iCDF;
            } else if( fs_kHz == 12 ) {
                psDec->pitch_lag_low_bits_iCDF = silk_uniform6_iCDF;
            } else if( fs_kHz == 8 ) {
                psDec->pitch_lag_low_bits_iCDF = silk_uniform4_iCDF;
            } else {
                /* unsupported sampling rate */
                silk_assert( 0 );
            }
            psDec->first_frame_after_reset = 1;
            psDec->lagPrev                 = 100;
            psDec->LastGainIndex           = 10;
            psDec->prevSignalType          = TYPE_NO_VOICE_ACTIVITY;
        }

        psDec->fs_kHz       = fs_kHz;
        psDec->frame_length = frame_length;
    }

    /* Check that settings are valid */
    silk_assert( psDec->frame_length > 0 && psDec->frame_length <= MAX_FRAME_LENGTH );

    return ret;
}

