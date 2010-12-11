#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "ffImgfmt.h"

extern "C" short bgr_to_yv12_mmx_data;
extern "C" short yv12_to_bgr_mmx_data;

struct TYCbCr2RGB_coeffs;

void xvid_colorspace_init(const TYCbCr2RGB_coeffs &coeffs);

typedef struct {
    uint8_t * y;
    uint8_t * u;
    uint8_t * v;
} IMAGE;

int image_input(IMAGE * image,
                uint32_t width,
                int height,
                stride_t edged_width,stride_t edged_width2,
                const uint8_t * src,
                stride_t src_stride,
                int csp,
                int interlaced,
                int jpeg,
                int outputLevelsMode);

int image_output(IMAGE * image,
                 uint32_t width,
                 int height,
                 stride_t edged_width[4],
                 uint8_t * dst[4],
                 stride_t dst_stride[4],
                 int csp,
                 int interlaced,
                 int jpeg,
                 int outputLevelsMode,
                 bool vram_indirect=false);

extern "C" {
#ifndef WIN64
    void yv12_to_TVbgr_mmx_asm (uint8_t * x_ptr,
                                int x_stride,
                                uint8_t * y_src,
                                uint8_t * v_src,
                                uint8_t * u_src,
                                int y_stride,
                                int uv_stride,
                                int width,
                                int height,
                                int vflip);

    void yv12_to_PCbgr_mmx_asm (uint8_t * x_ptr,
                                int x_stride,
                                uint8_t * y_src,
                                uint8_t * v_src,
                                uint8_t * u_src,
                                int y_stride,
                                int uv_stride,
                                int width,
                                int height,
                                int vflip);

    void yv12_to_TVbgra_mmx_asm (uint8_t * x_ptr,
                                 int x_stride,
                                 uint8_t * y_src,
                                 uint8_t * v_src,
                                 uint8_t * u_src,
                                 int y_stride,
                                 int uv_stride,
                                 int width,
                                 int height,
                                 int vflip);

    void yv12_to_PCbgra_mmx_asm (uint8_t * x_ptr,
                                 int x_stride,
                                 uint8_t * y_src,
                                 uint8_t * v_src,
                                 uint8_t * u_src,
                                 int y_stride,
                                 int uv_stride,
                                 int width,
                                 int height,
                                 int vflip);

    void bgr_to_yv12_mmx_asm (uint8_t * x_ptr,
                              int x_stride,
                              uint8_t * y_src,
                              uint8_t * v_src,
                              uint8_t * u_src,
                              int y_stride,
                              int uv_stride,
                              int width,
                              int height,
                              int vflip);

    void bgra_to_yv12_mmx_asm (uint8_t * x_ptr,
                               int x_stride,
                               uint8_t * y_src,
                               uint8_t * v_src,
                               uint8_t * u_src,
                               int y_stride,
                               int uv_stride,
                               int width,
                               int height,
                               int vflip);

#else //WIN64
    void yv12_to_TVbgr_win64_sse2a (uint8_t * x_ptr,
                                    size_t x_stride,
                                    uint8_t * y_src,
                                    uint8_t * v_src,
                                    uint8_t * u_src,
                                    size_t y_stride,
                                    size_t uv_stride,
                                    size_t width,
                                    size_t height,
                                    size_t vflip);

    void yv12_to_TVbgra_win64_sse2a (uint8_t * x_ptr,
                                     size_t x_stride,
                                     uint8_t * y_src,
                                     uint8_t * v_src,
                                     uint8_t * u_src,
                                     size_t y_stride,
                                     size_t uv_stride,
                                     size_t width,
                                     size_t height,
                                     size_t vflip);

    void yv12_to_TVbgr_win64_sse2u (uint8_t * x_ptr,
                                    size_t x_stride,
                                    uint8_t * y_src,
                                    uint8_t * v_src,
                                    uint8_t * u_src,
                                    size_t y_stride,
                                    size_t uv_stride,
                                    size_t width,
                                    size_t height,
                                    size_t vflip);

    void yv12_to_TVbgra_win64_sse2u (uint8_t * x_ptr,
                                     size_t x_stride,
                                     uint8_t * y_src,
                                     uint8_t * v_src,
                                     uint8_t * u_src,
                                     size_t y_stride,
                                     size_t uv_stride,
                                     size_t width,
                                     size_t height,
                                     size_t vflip);

    void yv12_to_PCbgr_win64_sse2a (uint8_t * x_ptr,
                                    size_t x_stride,
                                    uint8_t * y_src,
                                    uint8_t * v_src,
                                    uint8_t * u_src,
                                    size_t y_stride,
                                    size_t uv_stride,
                                    size_t width,
                                    size_t height,
                                    size_t vflip);

    void yv12_to_PCbgra_win64_sse2a (uint8_t * x_ptr,
                                     size_t x_stride,
                                     uint8_t * y_src,
                                     uint8_t * v_src,
                                     uint8_t * u_src,
                                     size_t y_stride,
                                     size_t uv_stride,
                                     size_t width,
                                     size_t height,
                                     size_t vflip);

    void yv12_to_PCbgr_win64_sse2u (uint8_t * x_ptr,
                                    size_t x_stride,
                                    uint8_t * y_src,
                                    uint8_t * v_src,
                                    uint8_t * u_src,
                                    size_t y_stride,
                                    size_t uv_stride,
                                    size_t width,
                                    size_t height,
                                    size_t vflip);

    void yv12_to_PCbgra_win64_sse2u (uint8_t * x_ptr,
                                     size_t x_stride,
                                     uint8_t * y_src,
                                     uint8_t * v_src,
                                     uint8_t * u_src,
                                     size_t y_stride,
                                     size_t uv_stride,
                                     size_t width,
                                     size_t height,
                                     size_t vflip);

    void bgr_to_yv12_win64_sse2 (uint8_t * x_ptr,
                                 size_t x_stride,
                                 uint8_t * y_src,
                                 uint8_t * v_src,
                                 uint8_t * u_src,
                                 size_t y_stride,
                                 size_t uv_stride,
                                 size_t width,
                                 size_t height,
                                 size_t vflip);

    void bgra_to_yv12_win64_sse2 (uint8_t * x_ptr,
                                  size_t x_stride,
                                  uint8_t * y_src,
                                  uint8_t * v_src,
                                  uint8_t * u_src,
                                  size_t y_stride,
                                  size_t uv_stride,
                                  size_t width,
                                  size_t height,
                                  size_t vflip);

#endif //WIN64
}

#endif /* _IMAGE_H_ */
