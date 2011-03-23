/*
 * The simplest AC-3 encoder
 * Copyright (c) 2000 Fabrice Bellard
 * Copyright (c) 2006-2010 Justin Ruggles <justin.ruggles@gmail.com>
 * Copyright (c) 2006-2010 Prakash Punnoor <prakash@punnoor.de>
 *
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * fixed-point AC-3 encoder.
 */

#undef CONFIG_AC3ENC_FLOAT
#include "ac3enc.c"


/** Scale a float value by 2^15, convert to an integer, and clip to range -32767..32767. */
#define FIX15(a) av_clip(SCALE_FLOAT(a, 15), -32767, 32767)


/**
 * Finalize MDCT and free allocated memory.
 */
static av_cold void mdct_end(AC3MDCTContext *mdct)
{
    mdct->nbits = 0;
    av_freep(&mdct->costab);
    av_freep(&mdct->sintab);
    av_freep(&mdct->xcos1);
    av_freep(&mdct->xsin1);
    av_freep(&mdct->rot_tmp);
    av_freep(&mdct->cplx_tmp);
}


/**
 * Initialize FFT tables.
 * @param ln log2(FFT size)
 */
static av_cold int fft_init(AVCodecContext *avctx, AC3MDCTContext *mdct, int ln)
{
    int i, n, n2;
    float alpha;

    n  = 1 << ln;
    n2 = n >> 1;

    FF_ALLOC_OR_GOTO(avctx, mdct->costab, n2 * sizeof(*mdct->costab), fft_alloc_fail);
    FF_ALLOC_OR_GOTO(avctx, mdct->sintab, n2 * sizeof(*mdct->sintab), fft_alloc_fail);

    for (i = 0; i < n2; i++) {
        alpha     = 2.0 * M_PI * i / n;
        mdct->costab[i] = FIX15(cos(alpha));
        mdct->sintab[i] = FIX15(sin(alpha));
    }

    return 0;
fft_alloc_fail:
    mdct_end(mdct);
    return AVERROR(ENOMEM);
}


/**
 * Initialize MDCT tables.
 * @param nbits log2(MDCT size)
 */
static av_cold int mdct_init(AVCodecContext *avctx, AC3MDCTContext *mdct,
                             int nbits)
{
    int i, n, n4, ret;

    n  = 1 << nbits;
    n4 = n >> 2;

    mdct->nbits = nbits;

    ret = fft_init(avctx, mdct, nbits - 2);
    if (ret)
        return ret;

    mdct->window = ff_ac3_window;

    FF_ALLOC_OR_GOTO(avctx, mdct->xcos1,    n4 * sizeof(*mdct->xcos1),    mdct_alloc_fail);
    FF_ALLOC_OR_GOTO(avctx, mdct->xsin1,    n4 * sizeof(*mdct->xsin1),    mdct_alloc_fail);
    FF_ALLOC_OR_GOTO(avctx, mdct->rot_tmp,  n  * sizeof(*mdct->rot_tmp),  mdct_alloc_fail);
    FF_ALLOC_OR_GOTO(avctx, mdct->cplx_tmp, n4 * sizeof(*mdct->cplx_tmp), mdct_alloc_fail);

    for (i = 0; i < n4; i++) {
        float alpha = 2.0 * M_PI * (i + 1.0 / 8.0) / n;
        mdct->xcos1[i] = FIX15(-cos(alpha));
        mdct->xsin1[i] = FIX15(-sin(alpha));
    }

    return 0;
mdct_alloc_fail:
    mdct_end(mdct);
    return AVERROR(ENOMEM);
}


/** Butterfly op */
#define BF(pre, pim, qre, qim, pre1, pim1, qre1, qim1)  \
{                                                       \
  int ax, ay, bx, by;                                   \
  bx  = pre1;                                           \
  by  = pim1;                                           \
  ax  = qre1;                                           \
  ay  = qim1;                                           \
  pre = (bx + ax) >> 1;                                 \
  pim = (by + ay) >> 1;                                 \
  qre = (bx - ax) >> 1;                                 \
  qim = (by - ay) >> 1;                                 \
}


/** Complex multiply */
#define CMUL(pre, pim, are, aim, bre, bim, rshift)      \
{                                                       \
   pre = (MUL16(are, bre) - MUL16(aim, bim)) >> rshift; \
   pim = (MUL16(are, bim) + MUL16(bre, aim)) >> rshift; \
}


/**
 * Calculate a 2^n point complex FFT on 2^ln points.
 * @param z  complex input/output samples
 * @param ln log2(FFT size)
 */
static void fft(AC3MDCTContext *mdct, IComplex *z, int ln)
{
    int j, l, np, np2;
    int nblocks, nloops;
    register IComplex *p,*q;
    int tmp_re, tmp_im;

    np = 1 << ln;

    /* reverse */
    for (j = 0; j < np; j++) {
        int k = av_reverse[j] >> (8 - ln);
        if (k < j)
            FFSWAP(IComplex, z[k], z[j]);
    }

    /* pass 0 */

    p = &z[0];
    j = np >> 1;
    do {
        BF(p[0].re, p[0].im, p[1].re, p[1].im,
           p[0].re, p[0].im, p[1].re, p[1].im);
        p += 2;
    } while (--j);

    /* pass 1 */

    p = &z[0];
    j = np >> 2;
    do {
        BF(p[0].re, p[0].im, p[2].re,  p[2].im,
           p[0].re, p[0].im, p[2].re,  p[2].im);
        BF(p[1].re, p[1].im, p[3].re,  p[3].im,
           p[1].re, p[1].im, p[3].im, -p[3].re);
        p+=4;
    } while (--j);

    /* pass 2 .. ln-1 */

    nblocks = np >> 3;
    nloops  =  1 << 2;
    np2     = np >> 1;
    do {
        p = z;
        q = z + nloops;
        for (j = 0; j < nblocks; j++) {
            BF(p->re, p->im, q->re, q->im,
               p->re, p->im, q->re, q->im);
            p++;
            q++;
            for(l = nblocks; l < np2; l += nblocks) {
                CMUL(tmp_re, tmp_im, mdct->costab[l], -mdct->sintab[l], q->re, q->im, 15);
                BF(p->re, p->im, q->re,  q->im,
                   p->re, p->im, tmp_re, tmp_im);
                p++;
                q++;
            }
            p += nloops;
            q += nloops;
        }
        nblocks = nblocks >> 1;
        nloops  = nloops  << 1;
    } while (nblocks);
}


/**
 * Calculate a 512-point MDCT
 * @param out 256 output frequency coefficients
 * @param in  512 windowed input audio samples
 */
static void mdct512(AC3MDCTContext *mdct, int32_t *out, int16_t *in)
{
    int i, re, im, n, n2, n4;
    int16_t *rot = mdct->rot_tmp;
    IComplex *x  = mdct->cplx_tmp;

    n  = 1 << mdct->nbits;
    n2 = n >> 1;
    n4 = n >> 2;

    /* shift to simplify computations */
    for (i = 0; i <n4; i++)
        rot[i] = -in[i + 3*n4];
    memcpy(&rot[n4], &in[0], 3*n4*sizeof(*in));

    /* pre rotation */
    for (i = 0; i < n4; i++) {
        re =  ((int)rot[   2*i] - (int)rot[ n-1-2*i]) >> 1;
        im = -((int)rot[n2+2*i] - (int)rot[n2-1-2*i]) >> 1;
        CMUL(x[i].re, x[i].im, re, im, -mdct->xcos1[i], mdct->xsin1[i], 15);
    }

    fft(mdct, x, mdct->nbits - 2);

    /* post rotation */
    for (i = 0; i < n4; i++) {
        re = x[i].re;
        im = x[i].im;
        CMUL(out[n2-1-2*i], out[2*i], re, im, mdct->xsin1[i], mdct->xcos1[i], 0);
    }
}


/**
 * Apply KBD window to input samples prior to MDCT.
 */
static void apply_window(DSPContext *dsp, int16_t *output, const int16_t *input,
                         const int16_t *window, unsigned int len)
{
    dsp->apply_window_int16(output, input, window, len);
}


/**
 * Calculate the log2() of the maximum absolute value in an array.
 * @param tab input array
 * @param n   number of values in the array
 * @return    log2(max(abs(tab[])))
 */
static int log2_tab(AC3EncodeContext *s, int16_t *src, int len)
{
    int v = s->ac3dsp.ac3_max_msb_abs_int16(src, len);
    return av_log2(v);
}


/**
 * Normalize the input samples to use the maximum available precision.
 * This assumes signed 16-bit input samples.
 *
 * @return exponent shift
 */
static int normalize_samples(AC3EncodeContext *s)
{
    int v = 14 - log2_tab(s, s->windowed_samples, AC3_WINDOW_SIZE);
    if (v > 0)
        s->ac3dsp.ac3_lshift_int16(s->windowed_samples, AC3_WINDOW_SIZE, v);
    /* +6 to right-shift from 31-bit to 25-bit */
    return v + 6;
}


/**
 * Scale MDCT coefficients to 25-bit signed fixed-point.
 */
static void scale_coefficients(AC3EncodeContext *s)
{
    int blk, ch;

    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        AC3Block *block = &s->blocks[blk];
        for (ch = 0; ch < s->channels; ch++) {
            s->ac3dsp.ac3_rshift_int32(block->mdct_coef[ch], AC3_MAX_COEFS,
                                       block->coeff_shift[ch]);
        }
    }
}


AVCodec ff_ac3_fixed_encoder = {
    "ac3_fixed",
    AVMEDIA_TYPE_AUDIO,
    CODEC_ID_AC3,
    sizeof(AC3EncodeContext),
    ac3_encode_init,
    ac3_encode_frame,
    ac3_encode_close,
    NULL,
    .sample_fmts = (const enum AVSampleFormat[]){AV_SAMPLE_FMT_S16,AV_SAMPLE_FMT_NONE},
    .long_name = NULL_IF_CONFIG_SMALL("ATSC A/52A (AC-3)"),
    .channel_layouts = ac3_channel_layouts,
};
