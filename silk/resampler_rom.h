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

#ifndef SILK_FIX_RESAMPLER_ROM_H
#define SILK_FIX_RESAMPLER_ROM_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include "typedef.h"
#include "resampler_structs.h"

#define RESAMPLER_DOWN_ORDER_FIR                16
#define RESAMPLER_ORDER_FIR_144                 6

/* Tables for 2x downsampler. Values above 32767 intentionally wrap to a negative value. */
extern const opus_int16 silk_resampler_down2_0;
extern const opus_int16 silk_resampler_down2_1;

/* Tables for 2x upsampler, high quality. Values above 32767 intentionally wrap to a negative value. */
extern const opus_int16 silk_resampler_up2_hq_0[ 2 ];
extern const opus_int16 silk_resampler_up2_hq_1[ 2 ];
extern const opus_int16 silk_resampler_up2_hq_notch[ 4 ];

/* Tables with IIR and FIR coefficients for fractional downsamplers */
extern const opus_int16 silk_Resampler_3_4_COEFS[ 2 + 3 * RESAMPLER_DOWN_ORDER_FIR / 2 ];
extern const opus_int16 silk_Resampler_2_3_COEFS[ 2 + 2 * RESAMPLER_DOWN_ORDER_FIR / 2 ];
extern const opus_int16 silk_Resampler_1_2_COEFS[ 2 +     RESAMPLER_DOWN_ORDER_FIR / 2 ];
extern const opus_int16 silk_Resampler_1_3_COEFS[ 2 +     RESAMPLER_DOWN_ORDER_FIR / 2 ];
extern const opus_int16 silk_Resampler_2_3_COEFS_LQ[ 2 + 2 * 2 ];

/* Table with interplation fractions of 1/288 : 2/288 : 287/288 (432 Words) */
extern const opus_int16 silk_resampler_frac_FIR_144[ 144 ][ RESAMPLER_ORDER_FIR_144 / 2 ];

#ifdef  __cplusplus
}
#endif

#endif /* SILK_FIX_RESAMPLER_ROM_H */
