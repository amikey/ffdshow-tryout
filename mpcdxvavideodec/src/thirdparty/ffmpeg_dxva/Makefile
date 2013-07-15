# (C) 2007-2013 see Authors.txt
#
# This file is part of MPC-HC.
#
# MPC-HC is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# MPC-HC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC = gcc.exe

ifeq ($(VS2012),yes)
    BIN_DIR  = ../../../bin12
else
    BIN_DIR  = ../../../bin
endif
ZLIB_DIR = ../zlib

ifeq ($(DEBUG),yes)
    OUT_DIR_PREFIX = Debug
else
    OUT_DIR_PREFIX = Release
endif

ifeq ($(64BIT),yes)
    MY_ARCH = x64
else
    MY_ARCH = Win32
endif


OBJ_DIR     = $(BIN_DIR)/obj/$(OUT_DIR_PREFIX)_$(MY_ARCH)/ffmpeg_dxva/
OUT_LIB_DIR = $(BIN_DIR)/lib/$(OUT_DIR_PREFIX)_$(MY_ARCH)
OUT_LIB     = $(OUT_LIB_DIR)/ffmpeg_dxva.lib


# Compiler and yasm flags
CFLAGS    = -I. -I.. -I$(ZLIB_DIR) -DHAVE_AV_CONFIG_H -D_ISOC99_SOURCE \
            -D_XOPEN_SOURCE=600 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
            -std=gnu99 -pipe -mthreads
OPTFLAGS  = -O2 -fno-common -fno-tree-vectorize -fomit-frame-pointer
YASMFLAGS = -I. -Pconfig.asm

ifeq ($(64BIT),yes)
    GCC_PREFIX = x86_64-w64-mingw32-
    TARGET_OS  = x86_64-w64-mingw32
    CFLAGS    += -DWIN64 -D_WIN64 -DARCH_X86_64 -DPIC
    OPTFLAGS  += -m64 -fno-leading-underscore
    YASMFLAGS += -f win64 -m amd64 -DWIN64=1 -DARCH_X86_32=0 -DARCH_X86_64=1 -DPIC
else
    TARGET_OS  = i686-w64-mingw32
    CFLAGS    += -DWIN32 -D_WIN32 -DARCH_X86_32
    OPTFLAGS  += -m32 -march=i686 -mmmx -msse -mfpmath=sse
    YASMFLAGS += -f win32 -m x86 -DWIN32=1 -DARCH_X86_32=1 -DARCH_X86_64=0 -DPREFIX
endif

ifeq ($(DEBUG),yes)
    CFLAGS    += -DDEBUG -D_DEBUG -g
else
    CFLAGS    += -DNDEBUG -UDEBUG -U_DEBUG
endif


# Object directories
OBJ_DIRS = \
    $(OBJ_DIR)libavcodec \
    $(OBJ_DIR)libavcodec/x86 \
    $(OBJ_DIR)libavresample \
    $(OBJ_DIR)libavresample/x86 \
    $(OBJ_DIR)libavutil \
    $(OBJ_DIR)libavutil/x86 \
    $(OBJ_DIR)libswscale \
    $(OBJ_DIR)libswscale/x86 \

# Targets
all: create_dirs $(OUT_LIB)

create_dirs: $(OBJ_DIRS) $(OUT_LIB_DIR)
$(OBJ_DIRS) $(OUT_LIB_DIR):
	$(shell test -d $(@) || mkdir -p $(@))

clean:
	@echo Cleaning ffmpeg...
	rm -f \
      $(OUT_LIB) \
      $(OBJ_DIR)mpc_helper.o \
      $(OBJ_DIR)mpc_helper.d
	rm -rf $(OBJ_DIRS)


# Objects
SRCS_C = \
    mpc_helper.c \
    libavcodec/allcodecs.c \
    libavcodec/avpacket.c \
    libavcodec/avpicture.c \
    libavcodec/bitstream.c \
    libavcodec/cabac.c \
    libavcodec/codec_desc.c \
    libavcodec/dsputil.c \
    libavcodec/error_resilience.c \
    libavcodec/faanidct.c \
    libavcodec/frame_thread_encoder.c \
    libavcodec/golomb.c \
    libavcodec/h263.c \
    libavcodec/h263_parser.c \
    libavcodec/h263dec.c \
    libavcodec/h264.c \
    libavcodec/h264_cabac.c \
    libavcodec/h264_cavlc.c \
    libavcodec/h264_direct.c \
    libavcodec/h264_loopfilter.c \
    libavcodec/h264_parser.c \
    libavcodec/h264_ps.c \
    libavcodec/h264_refs.c \
    libavcodec/h264_sei.c \
    libavcodec/h264chroma.c \
    libavcodec/h264dsp.c \
    libavcodec/h264idct.c \
    libavcodec/h264pred.c \
    libavcodec/h264qpel.c \
    libavcodec/huffman.c \
    libavcodec/imgconvert.c \
    libavcodec/intrax8.c \
    libavcodec/intrax8dsp.c \
    libavcodec/ituh263dec.c \
    libavcodec/jrevdct.c \
    libavcodec/log2_tab.c \
    libavcodec/mathtables.c \
    libavcodec/mpeg4video.c \
    libavcodec/mpeg4videodec.c \
    libavcodec/mpegvideo.c \
    libavcodec/mpegvideo_motion.c \
    libavcodec/msmpeg4.c \
    libavcodec/msmpeg4data.c \
    libavcodec/options.c \
    libavcodec/parser.c \
    libavcodec/pthread.c \
    libavcodec/simple_idct.c \
    libavcodec/utils.c \
    libavcodec/vc1.c \
    libavcodec/vc1data.c \
    libavcodec/vc1dec.c \
    libavcodec/vc1dsp.c \
    libavcodec/videodsp.c \
    libavcodec/x86/dsputil_mmx.c \
    libavcodec/x86/h264_intrapred_init.c \
    libavcodec/x86/h264_qpel.c \
    libavcodec/x86/h264chroma_init.c \
    libavcodec/x86/h264dsp_init.c \
    libavcodec/x86/idct_mmx.c \
    libavcodec/x86/idct_mmx_xvid.c \
    libavcodec/x86/idct_sse2_xvid.c \
    libavcodec/x86/mpegvideo.c \
    libavcodec/x86/simple_idct.c \
    libavcodec/x86/vc1dsp_init.c \
    libavcodec/x86/vc1dsp_mmx.c \
    libavcodec/x86/videodsp_init.c \
\
    libavutil/audio_fifo.c \
    libavutil/avstring.c \
    libavutil/bprint.c \
    libavutil/channel_layout.c \
    libavutil/cpu.c \
    libavutil/crc.c \
    libavutil/dict.c \
    libavutil/error.c \
    libavutil/eval.c \
    libavutil/fifo.c \
    libavutil/imgutils.c \
    libavutil/intfloat_readwrite.c \
    libavutil/lfg.c \
    libavutil/log.c \
    libavutil/mathematics.c \
    libavutil/md5.c \
    libavutil/mem.c \
    libavutil/opt.c \
    libavutil/parseutils.c \
    libavutil/pixdesc.c \
    libavutil/random_seed.c \
    libavutil/rational.c \
    libavutil/samplefmt.c \
    libavutil/sha.c \
    libavutil/time.c \
    libavutil/timecode.c \
    libavutil/utils.c \
    libavutil/x86/cpu.c

# Yasm objects
SRCS_YASM = \
    libavcodec/x86/deinterlace.asm \
    libavcodec/x86/dsputil.asm \
    libavcodec/x86/h264_chromamc.asm \
    libavcodec/x86/h264_chromamc_10bit.asm \
    libavcodec/x86/h264_deblock.asm \
    libavcodec/x86/h264_deblock_10bit.asm \
    libavcodec/x86/h264_idct.asm \
    libavcodec/x86/h264_idct_10bit.asm \
    libavcodec/x86/h264_intrapred.asm \
    libavcodec/x86/h264_intrapred_10bit.asm \
    libavcodec/x86/h264_qpel_8bit.asm \
    libavcodec/x86/h264_qpel_10bit.asm \
    libavcodec/x86/h264_weight.asm \
    libavcodec/x86/h264_weight_10bit.asm \
    libavcodec/x86/hpeldsp.asm \
    libavcodec/x86/mpeg4qpel.asm \
    libavcodec/x86/vc1dsp.asm \
    libavcodec/x86/videodsp.asm \
    libavutil/x86/cpuid.asm \
    libavutil/x86/emms.asm

OBJS = \
    $(SRCS_C:%.c=$(OBJ_DIR)%.o) \
    $(SRCS_YASM:%.asm=$(OBJ_DIR)%.o)


# Commands
$(OBJ_DIR)%.o: %.c
	@echo CC '  ' $<
	@$(GCC_PREFIX)$(CC) -c $(CFLAGS) $(OPTFLAGS) -MMD -o $@ $<

$(OBJ_DIR)%.o: %.asm
	@echo YASM: '  ' Assembling $<
	@yasm $(YASMFLAGS) -I$(<D)/ -o $@ $<

$(OUT_LIB): $(OBJS)
	@echo AR '  ' $@
	@$(GCC_PREFIX)ar rc $@ $(OBJS)

-include $(SRCS_C:%.c=$(OBJ_DIR)%.d)

.PHONY: clean create_dirs $(OBJ_DIRS) $(OUT_LIB_DIR)
