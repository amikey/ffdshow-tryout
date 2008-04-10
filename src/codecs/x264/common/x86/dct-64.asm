;*****************************************************************************
;* dct-64.asm: h264 encoder library
;*****************************************************************************
;* Copyright (C) 2003-2008 x264 project
;*
;* Authors: Laurent Aimar <fenrir@via.ecp.fr> (initial version)
;*          Min Chen <chenm001.163.com> (converted to nasm)
;*          Loren Merritt <lorenm@u.washington.edu> (dct8)
;*
;* This program is free software; you can redistribute it and/or modify
;* it under the terms of the GNU General Public License as published by
;* the Free Software Foundation; either version 2 of the License, or
;* (at your option) any later version.
;*
;* This program is distributed in the hope that it will be useful,
;* but WITHOUT ANY WARRANTY; without even the implied warranty of
;* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;* GNU General Public License for more details.
;*
;* You should have received a copy of the GNU General Public License
;* along with this program; if not, write to the Free Software
;* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
;*****************************************************************************

%include "x86inc.asm"

SECTION_RODATA
pw_32: times 8 dw 32

SECTION .text

INIT_XMM

%macro LOAD_DIFF_8P 5
    movq        %1, %4
    punpcklbw   %1, %3
    movq        %2, %5
    punpcklbw   %2, %3
    psubw       %1, %2
%endmacro

%macro SUMSUB_BA 2
    paddw   %1, %2
    paddw   %2, %2
    psubw   %2, %1
%endmacro

%macro SBUTTERFLY 4
    mova      m%4, m%2
    punpckl%1 m%2, m%3
    punpckh%1 m%4, m%3
    SWAP %3, %4
%endmacro

%macro TRANSPOSE8x8W 9
    SBUTTERFLY wd,  %1, %2, %9
    SBUTTERFLY wd,  %3, %4, %9
    SBUTTERFLY wd,  %5, %6, %9
    SBUTTERFLY wd,  %7, %8, %9
    SBUTTERFLY dq,  %1, %3, %9
    SBUTTERFLY dq,  %2, %4, %9
    SBUTTERFLY dq,  %5, %7, %9
    SBUTTERFLY dq,  %6, %8, %9
    SBUTTERFLY qdq, %1, %5, %9
    SBUTTERFLY qdq, %2, %6, %9
    SBUTTERFLY qdq, %3, %7, %9
    SBUTTERFLY qdq, %4, %8, %9
    SWAP %2, %5
    SWAP %4, %7
%endmacro

%macro STORE_DIFF_8P 4
    psraw       %1, 6
    movq        %2, %4
    punpcklbw   %2, %3
    paddsw      %1, %2
    packuswb    %1, %1  
    movq        %4, %1
%endmacro

SECTION .text

%macro DCT8_1D 10
    SUMSUB_BA  m%8, m%1 ; %8=s07, %1=d07
    SUMSUB_BA  m%7, m%2 ; %7=s16, %2=d16
    SUMSUB_BA  m%6, m%3 ; %6=s25, %3=d25
    SUMSUB_BA  m%5, m%4 ; %5=s34, %4=d34

    SUMSUB_BA  m%5, m%8 ; %5=a0, %8=a2
    SUMSUB_BA  m%6, m%7 ; %6=a1, %7=a3

    movdqa  m%9, m%1
    psraw   m%9, 1
    paddw   m%9, m%1
    paddw   m%9, m%2
    paddw   m%9, m%3 ; %9=a4

    movdqa  m%10, m%4
    psraw   m%10, 1
    paddw   m%10, m%4
    paddw   m%10, m%2
    psubw   m%10, m%3 ; %10=a7

    SUMSUB_BA  m%4, m%1
    psubw   m%1, m%3
    psubw   m%4, m%2
    psraw   m%3, 1
    psraw   m%2, 1
    psubw   m%1, m%3 ; %1=a5
    psubw   m%4, m%2 ; %4=a6

    SUMSUB_BA  m%6, m%5 ; %6=b0, %5=b4

    movdqa  m%2, m%10
    psraw   m%2, 2
    paddw   m%2, m%9 ; %2=b1
    psraw   m%9, 2
    psubw   m%9, m%10 ; %9=b7

    movdqa  m%3, m%7
    psraw   m%3, 1
    paddw   m%3, m%8 ; %3=b2
    psraw   m%8, 1
    psubw   m%8, m%7 ; %8=b6

    movdqa  m%7, m%4
    psraw   m%7, 2
    paddw   m%7, m%1 ; %7=b3
    psraw   m%1, 2
    psubw   m%4, m%1 ; %4=b5

    SWAP %1, %6, %4, %7, %8, %9
%endmacro

;-----------------------------------------------------------------------------
; void x264_sub8x8_dct8_sse2( int16_t dct[8][8], uint8_t *pix1, uint8_t *pix2 )
;-----------------------------------------------------------------------------
cglobal x264_sub8x8_dct8_sse2
    LOAD_DIFF_8P  m0, m8, m9, [parm2q+0*FENC_STRIDE], [parm3q+0*FDEC_STRIDE]
    LOAD_DIFF_8P  m1, m8, m9, [parm2q+1*FENC_STRIDE], [parm3q+1*FDEC_STRIDE]
    LOAD_DIFF_8P  m2, m8, m9, [parm2q+2*FENC_STRIDE], [parm3q+2*FDEC_STRIDE]
    LOAD_DIFF_8P  m3, m8, m9, [parm2q+3*FENC_STRIDE], [parm3q+3*FDEC_STRIDE]
    LOAD_DIFF_8P  m4, m8, m9, [parm2q+4*FENC_STRIDE], [parm3q+4*FDEC_STRIDE]
    LOAD_DIFF_8P  m5, m8, m9, [parm2q+5*FENC_STRIDE], [parm3q+5*FDEC_STRIDE]
    LOAD_DIFF_8P  m6, m8, m9, [parm2q+6*FENC_STRIDE], [parm3q+6*FDEC_STRIDE]
    LOAD_DIFF_8P  m7, m8, m9, [parm2q+7*FENC_STRIDE], [parm3q+7*FDEC_STRIDE]

    DCT8_1D       0,1,2,3,4,5,6,7,8,9
    TRANSPOSE8x8W 0,1,2,3,4,5,6,7,8
    DCT8_1D       0,1,2,3,4,5,6,7,8,9

    movdqa  [parm1q+0x00], m0
    movdqa  [parm1q+0x10], m1
    movdqa  [parm1q+0x20], m2
    movdqa  [parm1q+0x30], m3
    movdqa  [parm1q+0x40], m4
    movdqa  [parm1q+0x50], m5
    movdqa  [parm1q+0x60], m6
    movdqa  [parm1q+0x70], m7
    ret


%macro IDCT8_1D 10
    SUMSUB_BA  m%5, m%1 ; %5=a0, %1=a2
    movdqa  m%10, m%3
    psraw   m%3, 1
    psubw   m%3, m%7 ; %3=a4
    psraw   m%7, 1
    paddw   m%7, m%10 ; %7=a6

    movdqa  m%9, m%2
    psraw   m%9, 1
    paddw   m%9, m%2
    paddw   m%9, m%4
    paddw   m%9, m%6 ; %9=a7
    
    movdqa  m%10, m%6
    psraw   m%10, 1
    paddw   m%10, m%6
    paddw   m%10, m%8
    psubw   m%10, m%2 ; %10=a5

    psubw   m%2, m%4
    psubw   m%6, m%4
    paddw   m%2, m%8
    psubw   m%6, m%8
    psraw   m%4, 1
    psraw   m%8, 1
    psubw   m%2, m%4 ; %2=a3
    psubw   m%6, m%8 ; %6=a1

    SUMSUB_BA m%7, m%5 ; %7=b0, %5=b6
    SUMSUB_BA m%3, m%1 ; %3=b2, %1=b4

    movdqa  m%4, m%9
    psraw   m%4, 2
    paddw   m%4, m%6 ; %4=b1
    psraw   m%6, 2
    psubw   m%9, m%6 ; %9=b7

    movdqa  m%8, m%10
    psraw   m%8, 2
    paddw   m%8, m%2 ; %8=b3
    psraw   m%2, 2
    psubw   m%2, m%10 ; %2=b5

    SUMSUB_BA m%9, m%7 ; %9=c0, %7=c7
    SUMSUB_BA m%2, m%3 ; %2=c1, %3=c6
    SUMSUB_BA m%8, m%1 ; %8=c2, %1=c5
    SUMSUB_BA m%4, m%5 ; %4=c3, %5=c4

    SWAP %1, %9, %6
    SWAP %3, %8, %7
%endmacro

;-----------------------------------------------------------------------------
; void x264_add8x8_idct8_sse2( uint8_t *p_dst, int16_t dct[8][8] )
;-----------------------------------------------------------------------------
cglobal x264_add8x8_idct8_sse2
    movdqa  m0, [parm2q+0x00]
    movdqa  m1, [parm2q+0x10]
    movdqa  m2, [parm2q+0x20]
    movdqa  m3, [parm2q+0x30]
    movdqa  m4, [parm2q+0x40]
    movdqa  m5, [parm2q+0x50]
    movdqa  m6, [parm2q+0x60]
    movdqa  m7, [parm2q+0x70]

    IDCT8_1D      0,1,2,3,4,5,6,7,8,9
    TRANSPOSE8x8W 0,1,2,3,4,5,6,7,8
    paddw         m0, [pw_32 GLOBAL] ; rounding for the >>6 at the end
    IDCT8_1D      0,1,2,3,4,5,6,7,8,9
 
    pxor  m9, m9
    STORE_DIFF_8P m0, m8, m9, [parm1q+0*FDEC_STRIDE]
    STORE_DIFF_8P m1, m8, m9, [parm1q+1*FDEC_STRIDE]
    STORE_DIFF_8P m2, m8, m9, [parm1q+2*FDEC_STRIDE]
    STORE_DIFF_8P m3, m8, m9, [parm1q+3*FDEC_STRIDE]
    STORE_DIFF_8P m4, m8, m9, [parm1q+4*FDEC_STRIDE]
    STORE_DIFF_8P m5, m8, m9, [parm1q+5*FDEC_STRIDE]
    STORE_DIFF_8P m6, m8, m9, [parm1q+6*FDEC_STRIDE]
    STORE_DIFF_8P m7, m8, m9, [parm1q+7*FDEC_STRIDE]
    ret


