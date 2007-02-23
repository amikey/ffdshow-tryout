/*
 *
 *  rgb2rgb.h, Software RGB to RGB convertor
 *  pluralize by Software PAL8 to RGB convertor
 *               Software YUV to YUV convertor
 *               Software YUV to RGB convertor
 */

#ifndef RGB2RGB_INCLUDED
#define RGB2RGB_INCLUDED

#define FF_CSP_ONLY
#include "ffImgfmt.h"

/* A full collection of rgb to rgb(bgr) convertors */
extern void (*rgb24to32)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb24to16)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb24to15)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb32to24)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb32to16)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb32to15)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb15to16)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb15to24)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb15to32)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb16to15)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb16to24)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb16to32)(const uint8_t *src,uint8_t *dst,stride_t src_size);
extern void (*rgb24tobgr24)(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void (*rgb24tobgr16)(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void (*rgb24tobgr15)(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void (*rgb32tobgr32)(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void (*rgb32tobgr16)(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void (*rgb32tobgr15)(const uint8_t *src, uint8_t *dst, stride_t src_size);

extern void rgb24tobgr32(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb32tobgr24(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb16tobgr32(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb16tobgr24(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb16tobgr16(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb16tobgr15(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb15tobgr32(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb15tobgr24(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb15tobgr16(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb15tobgr15(const uint8_t *src, uint8_t *dst, stride_t src_size);
extern void rgb8tobgr8(const uint8_t *src, uint8_t *dst, stride_t src_size);


extern void palette8torgb32(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
extern void palette8tobgr32(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
extern void palette8torgb24(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
extern void palette8tobgr24(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
extern void palette8torgb16(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
extern void palette8tobgr16(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
extern void palette8torgb15(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);
extern void palette8tobgr15(const uint8_t *src, uint8_t *dst, long num_pixels, const uint8_t *palette);

/**
 *
 * height should be a multiple of 2 and width should be a multiple of 16 (if this is a
 * problem for anyone then tell me, and ill fix it)
 * chrominance data is only taken from every secound line others are ignored FIXME write HQ version
 */
//void uyvytoyv12(const uint8_t *src, uint8_t *ydst, uint8_t *udst, uint8_t *vdst,

/**
 *
 * height should be a multiple of 2 and width should be a multiple of 16 (if this is a
 * problem for anyone then tell me, and ill fix it)
 */
extern void (*yv12toyuy2)(const uint8_t *ysrc, const uint8_t *usrc, const uint8_t *vsrc, uint8_t *dst,
	long width, long height,
	stride_t lumStride, stride_t chromStride, stride_t dstStride);

/**
 *
 * width should be a multiple of 16
 */
extern void (*yuv422ptoyuy2)(const uint8_t *ysrc, const uint8_t *usrc, const uint8_t *vsrc, uint8_t *dst,
	long width, long height,
	stride_t lumStride, stride_t chromStride, stride_t dstStride);

/**
 *
 * height should be a multiple of 2 and width should be a multiple of 16 (if this is a
 * problem for anyone then tell me, and ill fix it)
 */
extern void (*yuy2toyv12)(const uint8_t *src, uint8_t *ydst, uint8_t *udst, uint8_t *vdst,
	long width, long height,
	stride_t lumStride, stride_t chromStride, stride_t srcStride);

/**
 *
 * height should be a multiple of 2 and width should be a multiple of 16 (if this is a
 * problem for anyone then tell me, and ill fix it)
 */
extern void (*yv12touyvy)(const uint8_t *ysrc, const uint8_t *usrc, const uint8_t *vsrc, uint8_t *dst,
	long width, long height,
	stride_t lumStride, stride_t chromStride, stride_t dstStride);

extern void (*yv12tovyuy)(const uint8_t *ysrc, const uint8_t *usrc, const uint8_t *vsrc, uint8_t *dst,
	long width, long height,
	stride_t lumStride, stride_t chromStride, stride_t dstStride);

extern void (*yv12toyvyu)(const uint8_t *ysrc, const uint8_t *usrc, const uint8_t *vsrc, uint8_t *dst,
	long width, long height,
	stride_t lumStride, stride_t chromStride, stride_t dstStride);

/**
 *
 * height should be a multiple of 2 and width should be a multiple of 2 (if this is a
 * problem for anyone then tell me, and ill fix it)
 * chrominance data is only taken from every secound line others are ignored FIXME write HQ version
 */
extern void (*rgb24toyv12)(const uint8_t *src, uint8_t *ydst, uint8_t *udst, uint8_t *vdst,
	long width, long height,
	stride_t lumStride, stride_t chromStride, stride_t srcStride);
extern void (*planar2x)(const uint8_t *src, uint8_t *dst, long width, long height,
	stride_t srcStride, stride_t dstStride);

extern void (*interleaveBytes)(uint8_t *src1, uint8_t *src2, uint8_t *dst,
			    long width, long height, stride_t src1Stride,
			    stride_t src2Stride, stride_t dstStride);

extern void (*vu9_to_vu12)(const uint8_t *src1, const uint8_t *src2,
			uint8_t *dst1, uint8_t *dst2,
			long width, long height,
			stride_t srcStride1, stride_t srcStride2,
			stride_t dstStride1, stride_t dstStride2);

extern void (*yvu9_to_yuy2)(const uint8_t *src1, const uint8_t *src2, const uint8_t *src3,
  			 uint8_t *dst,
			long width, long height,
			stride_t srcStride1, stride_t srcStride2,
			stride_t srcStride3, stride_t dstStride);

struct SwsParams;
void sws_rgb2rgb_init(struct SwsParams *params);

#endif
