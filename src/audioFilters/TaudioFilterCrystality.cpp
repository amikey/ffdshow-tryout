/*
 * Copyright (c) 2004-2006 Milan Cutka
 * derived from Crystality Plugin - A plugin for remastering mp3 sound in realtime, Copyright (C) 2001 Rafal Bosak
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TaudioFilterCrystality.h"

TaudioFilterCrystality::TaudioFilterCrystality(IffdshowBase *Ideci, Tfilters *Iparent): TaudioFilter(Ideci, Iparent)
{
    oldnchannels = 0;
}

TaudioFilterCrystality::~TaudioFilterCrystality()
{
}

// quite nice echo
void TaudioFilterCrystality::Techo3d::init(const TcrystalitySettings *cfg)
{
    echo_sfactor = cfg->echo_level;
    stereo_sfactor = cfg->stereo_level;
    feedback_sfactor = (cfg->feedback_level * 3) / 2;
    harmonics_sfactor = cfg->harmonics_level;

    static const int COND = 0;
    for (int i = 0; i < 32768; i++) {
        double lsum = 0, rsum = 0;
        if (COND || i < 32768) {
            lsum += ((cos((double)i * 3.141592535 / 32768 / 2)) + 0) / 2;
        }
        if (COND || i < 16384) {
            rsum -= ((cos((double)i * 3.141592535 / 16384 / 2)) + 0) / 4;
        }
        rsum = lsum;

        if (COND || i < 8192) {
            lsum += ((cos((double)i * 3.141592535 / 8192 / 2)) + 0) /  8;
        }
        if (COND || i < 5641) {
            rsum += ((cos((double)i * 3.141592535 / 5641.333333 / 2)) + 0) /  8;
        }
        //if (COND || i < 4096 ) lsum -= ((cos((double)i * 3.141592535 / 4096/2    )) + 0) /  16;
        //if (COND || i < 3276 ) rsum -= ((cos((double)i * 3.141592535 / 3276.8/2  )) + 0) /  16;
        //if (COND || i < 2730 ) lsum += ((cos((double)i * 3.141592535 / 2730.666667/2)) + 0) /  24;
        //if (COND || i < 2340 ) rsum += ((cos((double)i * 3.141592535 / 2340.571428/2)) + 0) /  24;

        lsine[32768 + i] = int((double)(lsum - 0.5) * 32768 * 1.45);
        lsine[32768 - i] = lsine[32768 + i];
        rsine[32768 + i] = int((double)(rsum - 0.5) * 32768 * 1.45);
        rsine[32768 - i] = rsine[32768 + i];
        //x = i;
    }
}

void TaudioFilterCrystality::Techo3d::process(int16_t *data, size_t datasize)
{
    int left, right, dif, left0, right0, left1, right1, left2, right2, left3, right3, left4, right4, leftc, rightc, lsf, rsf;
    int16_t *dataptr;
    int lharm0, rharm0;
    dataptr = data;

    for (size_t x = 0; x < datasize; x += 4) {
        // ************ load sample **********
        left0 = dataptr[0];
        right0 = dataptr[1];

        // ************ slightly expand stereo for direct input **********
        ll0 = left0;
        rr0 = right0;
        dif = (ll0 + ll1 + ll2 - rr0 - rr1 - rr2) * stereo_sfactor / 256;
        left0 += dif;
        right0 -= dif;
        ll2 = ll1;
        ll1 = ll0;
        rr2 = rr1;
        rr1 = rr0;

        // ************ echo from buffer - first echo **********
        // ************  **********
        left1 = buf[bufPos1++];
        if (bufPos1 == BUF_SIZE) {
            bufPos1 = 0;
        }
        right1 = buf[bufPos1++];
        if (bufPos1 == BUF_SIZE) {
            bufPos1 = 0;
        }

        // ************ highly expand stereo for first echo **********
        dif = (left1 - right1);
        left1 = left1 + dif;
        right1 = right1 - dif;

        // ************ second echo  **********
        left2 = buf[bufPos2++];
        if (bufPos2 == BUF_SIZE) {
            bufPos2 = 0;
        }
        right2 = buf[bufPos2++];
        if (bufPos2 == BUF_SIZE) {
            bufPos2 = 0;
        }

        // ************ expand stereo for second echo **********
        dif = (left2 - right2);
        left2 = left2 - dif;
        right2 = right2 - dif;

        // ************ third echo  **********
        left3 = buf[bufPos3++];
        if (bufPos3 == BUF_SIZE) {
            bufPos3 = 0;
        }
        right3 = buf[bufPos3++];
        if (bufPos3 == BUF_SIZE) {
            bufPos3 = 0;
        }

        // ************ fourth echo  **********
        left4 = buf[bufPos4++];
        if (bufPos4 == BUF_SIZE) {
            bufPos4 = 0;
        }
        right4 = buf[bufPos4++];
        if (bufPos4 == BUF_SIZE) {
            bufPos4 = 0;
        }

        left3 = (left4 + left3) / 2;
        right3 = (right4 + right3) / 2;

        // ************ expand stereo for second echo **********
        dif = (left4 - right4);
        left3 = left3 - dif;
        right3 = right3 - dif;

        // ************ a weighted sum taken from reverb buffer **********
        leftc = left1 / 9 + right2 / 8  + left3 / 8;
        rightc = right1 / 11 + left2 / 9 + right3 / 10;

        // ************ mix reverb with (near to) direct input **********
        //left =  leftc * echo_sfactor / 32;
        //right =  rightc * echo_sfactor / 32;
        left = left0p + leftc * echo_sfactor / 16;
        right = right0p + rightc * echo_sfactor / 16;

        l0 = leftc + left0 / 2;
        r0 = rightc + right0 / 2;

        ls = l0 + l1 + l2;  // do not reverb high frequencies (filter)
        rs = r0 + r1 + r2;  //

        buf[bufPos++] = int16_t(ls * feedback_sfactor / 256);
        if (bufPos == BUF_SIZE) {
            bufPos = 0;
        }
        buf[bufPos++] = int16_t(rs * feedback_sfactor / 256);
        if (bufPos == BUF_SIZE) {
            bufPos = 0;
        }

        // ************ add some extra even harmonics **********
        // ************ or rather specific nonlinearity

        lhfb = lhfb + (ls * 32768 - lhfb) / 32;
        rhfb = rhfb + (rs * 32768 - rhfb) / 32;

        lsf = ls - lhfb / 32768;
        rsf = rs - rhfb / 32768;

        lharm0 = 0
                 + ((lsf + 10000) * ((((lsine[((lsf / 4) + 32768 + 65536) % 65536] * harmonics_sfactor)) / 64))) / 32768
                 - ((lsine[((lsf / 4) + 32768 + 65536) % 65536]) * harmonics_sfactor) / 128
                 ;
        rharm0 =
            + ((rsf + 10000) * ((((lsine[((rsf / 4) + 32768 + 65536) % 65536] * harmonics_sfactor)) / 64))) / 32768
            - ((rsine[((rsf / 4) + 32768 + 65536) % 65536]) * harmonics_sfactor) / 128
            ;

        lharmb = lharmb + (lharm0 * 32768 - lharmb) / 16384;
        rharmb = rharmb + (rharm0 * 32768 - rharmb) / 16384;

        // ************ for convolution filters **********
        l2 = l1;
        r2 = r1;
        l1 = l0;
        r1 = r0;
        ls1 = ls;
        rs1 = rs;

        left  = 0
                // +lsf
                + lharm0 - lharmb / 32768
                + left
                ;
        right = 0
                // +rsf
                + rharm0 - rharmb / 32768
                + right
                ;

        left0p = left0;
        right0p = right0;

        // ************ store sample **********
        dataptr[0] = TsampleFormatInfo<int16_t>::limit(left);
        dataptr[1] = TsampleFormatInfo<int16_t>::limit(right);
        dataptr += 2;
    }
}

void TaudioFilterCrystality::Techo3d::onSeek(void)
{
    left0p = 0;
    right0p = 0;
    lharmb = 0;
    rharmb = 0;
    lhfb = 0;
    rhfb = 0;
    l0 = l1 = l2 = r0 = r1 = r2 = ls = rs = ls1 = rs1 = 0;
    ll0 = ll1 = ll2 = rr0 = rr1 = rr2 = 0;
    bufPos = BUF_SIZE - 1;
    bufPos1 = 1 + BUF_SIZE - DELAY1;
    bufPos2 = 1 + BUF_SIZE - DELAY1 - DELAY2;
    bufPos3 = 1 + BUF_SIZE - DELAY1 - DELAY2 - DELAY3;
    bufPos4 = 1 + BUF_SIZE - DELAY1 - DELAY2 - DELAY3 - DELAY4;
}

// simple pith shifter, plays short fragments at 1.5 speed
void TaudioFilterCrystality::Tbandext::pitchShifter(const int &in, int *out)
{
    shBuf[shBufPos++] = in;

    if (shBufPos == SH_BUF_SIZE) {
        shBufPos = 0;
    }

    switch (cond) {
        case 1:
            *out = (shBuf[shBufPos1 + 0] * 2 + shBuf[shBufPos1 + 1]) / 4;
            break;
        case 0:
            *out = (shBuf[shBufPos1 + 2] * 2 + shBuf[shBufPos1 + 1]) / 4;
            cond = 2;
            shBufPos1 += 3;
            if (shBufPos1 == SH_BUF_SIZE) {
                shBufPos1 = 0;
            }
            break;
    }
    cond--;
}

//template<unsigned int N> typename TaudioFilterCrystality::Tbandext::Tn operator /(TaudioFilterCrystality::Tbandext::Tn a,int b);
// interpolation routine for amplitude and "energy"
void TaudioFilterCrystality::Tbandext::interpolate(Interpolation *s, int l)
{
    static const int AMPL_COUNT = 64;
    int a0l, dal = 0;

    if (l < 0) {
        l = -l;
    }

    s->lval += l / 8;

    s->lval = (s->lval * 120) / 128;

    s->sal += s->lval;

    s->acount++;
    if (s->acount == AMPL_COUNT) {
        s->acount = 0;
        a0l = s->a1l;
        s->a1l = s->sal / AMPL_COUNT;
        s->sal = 0;
        dal = s->a1l - a0l;
        s->al = a0l * AMPL_COUNT;
    }

    s->al += dal;
}

// calculate scalefactor for mixer
int TaudioFilterCrystality::Tbandext::calc_scalefactor(int a, int e)
{
    a = limit(a, 0, 8192);
    e = limit(e, 0, 8192);

    int x = limit(((e + 500) * 4096) / (a + 300) + e, 0, 16384);
    return x;
}

void TaudioFilterCrystality::Tbandext::init(const TcrystalitySettings *cfg)
{
    filter_level = cfg->filter_level;
    bext_sfactor = int((float)(((float)16384 * 10) / (float)(cfg->bext_level + 1)) + (float)(102 - cfg->bext_level) * 128);
}

// exact bandwidth extender ("exciter") routine
void TaudioFilterCrystality::Tbandext::process(int16_t *dataptr, const size_t datasize, unsigned int N)
{
    int left = 0;

    for (size_t x = 0; x < datasize; x += sizeof(*dataptr) * N) {
        // ************ load sample **********
        int left0 = *dataptr;
        // ************ highpass filter part 1 **********
        int left1  = (left0  - prev0) * 56880 / 65536;
        int left2  = (left1  - prev1) * 56880 / 65536;
        int left3  = (left2  - prev2) * 56880 / 65536;

        switch (filter_level) {
            case 1:
                pitchShifter(left1, &left);
                break;
            case 2:
                pitchShifter(left2, &left);
                break;
            case 3:
                pitchShifter(left3, &left);
                break;
        }

        // ************ amplitude detector ************
        int lampl = 0;
        for (unsigned int i = 0; i < N; i++) {
            int tmp = left1 + prev1;
            if (tmp * 16 > amplUp) {
                amplUp   += (tmp - amplUp);
            } else if (tmp * 16 < amplDown) {
                amplDown += (tmp - amplDown);
            }
            amplUp   = (amplUp   * 1000) / 1024;
            amplDown = (amplDown * 1000) / 1024;
            lampl = amplUp - amplDown;
        }

        interpolate(&bandext_amplitude, lampl);

        // ************ "sound energy" detector (approx. spectrum complexity) ***********
        interpolate(&bandext_energy, left0  - prev0);

        // ************ mixer ***********
        left   = left0  + left  * calc_scalefactor(bandext_amplitude.lval, bandext_energy.lval) / bext_sfactor;

        // ************ highpass filter part 2 **********
        // ************ save previous values for filter
        prev0 = left0;
        prev1 = left1;
        prev2 = left2;
        // ************ END highpass filter part 2 **********

        // ************ store sample **********
        *dataptr = TsampleFormatInfo<int16_t>::limit(left);
        dataptr += N;
    }
}

void TaudioFilterCrystality::Tbandext::onSeek(void)
{
    //bext_level=-1;
    shBufPos = SH_BUF_SIZE - 6;
    shBufPos1 = SH_BUF_SIZE - 6;
    memset(&bandext_energy, 0, sizeof(bandext_energy));
    memset(&bandext_amplitude, 0, sizeof(bandext_amplitude));
    memset(shBuf, 0, sizeof(shBuf));
    cond = 0;
    prev0 = prev1 = prev2 = 0;
    amplUp = amplDown = 0;
}

HRESULT TaudioFilterCrystality::process(TfilterQueue::iterator it, TsampleFormat &fmt, void *samples0, size_t numsamples, const TfilterSettingsAudio *cfg0)
{
    const TcrystalitySettings *cfg = (const TcrystalitySettings*)cfg0;

    if (oldnchannels != fmt.nchannels || !cfg->equal(old)) {
        old = *cfg;
        oldnchannels = fmt.nchannels;
        if (fmt.nchannels == 2) {
            echo3d.init(cfg);
        }
        for (int i = 0; i < 8; i++) {
            bandext[i].init(cfg);
        }
    }

    if (is(fmt, cfg)) {
        int16_t *samples = (int16_t*)(samples0 = init(cfg, fmt, samples0, numsamples));
        if (fmt.nchannels == 2) {
            echo3d.process(samples, numsamples * 4);
        }
        for (unsigned int i = 0; i < fmt.nchannels; i++) {
            bandext[i].process(samples + i, numsamples * fmt.nchannels * sizeof(int16_t), fmt.nchannels);
        }
    }

    return parent->deliverSamples(++it, fmt, samples0, numsamples);
}

void TaudioFilterCrystality::onSeek(void)
{
    for (int i = 0; i < 8; i++) {
        bandext[i].onSeek();
    }
    echo3d.onSeek();
}
