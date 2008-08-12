#if defined(__GNUC__)
/* used for inline assembly stuff with GCC syntax */
  #define HAVE_MMX 1
  #define ENABLE_MMX 1
  #define HAVE_SSSE3 1
  #define ENABLE_SSSE3 1

  #define ARCH_X86 1

  #ifdef ARCH_X86_64
    #define HAVE_FAST_64BIT 1
  #else
    #define ARCH_X86_32 1
  #endif
#else
	#define ENABLE_MMX 0
	#define ENABLE_SSSE3 0
#endif


#define HAVE_MALLOC_H 1
#define HAVE_LRINTF 1
#define SIMPLE_IDCT 1
#define HAVE_W32THREADS 1
#define HAVE_THREADS 1
#define HAVE_MEMALIGN 1

#define ASMALIGN(ZEROBITS) ".align 1<<" #ZEROBITS "\n\t"

#define HAVE_BSWAP 1
#define ENABLE_BSWAP 1

#define HAVE_YASM 1
#define ENABLE_YASM 1

#define HAVE_FAST_UNALIGNED 1
#define ENABLE_FAST_UNALIGNED 1


#define CONFIG_DECODERS 1
#define ENABLE_DECODERS 1

#define CONFIG_ENCODERS 1
#define ENABLE_ENCODERS 1

#define CONFIG_GPL 1
#define ENABLE_GPL 1

#define CONFIG_ZLIB 1
#define ENABLE_ZLIB 1

#define ENABLE_SMALL 0

#define ENABLE_GRAY 1


#define CONFIG_AASC_DECODER 1
#define ENABLE_AASC_DECODER 1
#define CONFIG_AMV_DECODER 1
#define ENABLE_AMV_DECODER 1
#define CONFIG_ASV1_DECODER 1
#define ENABLE_ASV1_DECODER 1
#define CONFIG_ASV2_DECODER 1
#define ENABLE_ASV2_DECODER 1
#define CONFIG_AVS_DECODER 1
#define ENABLE_AVS_DECODER 1
#define CONFIG_CAVS_DECODER 1
#define ENABLE_CAVS_DECODER 1
#define CONFIG_CINEPAK_DECODER 1
#define ENABLE_CINEPAK_DECODER 1
#define CONFIG_CSCD_DECODER 1
#define ENABLE_CSCD_DECODER 1
#define CONFIG_CYUV_DECODER 1
#define ENABLE_CYUV_DECODER 1
#define CONFIG_DVVIDEO_DECODER 1
#define ENABLE_DVVIDEO_DECODER 1
#define CONFIG_EIGHTBPS_DECODER 1
#define ENABLE_EIGHTBPS_DECODER 1
#define CONFIG_FFV1_DECODER 1
#define ENABLE_FFV1_DECODER 1
#define CONFIG_FFVHUFF_DECODER 1
#define ENABLE_FFVHUFF_DECODER 1
#define CONFIG_FLV_DECODER 1
#define ENABLE_FLV_DECODER 1
#define CONFIG_FRAPS_DECODER 1
#define ENABLE_FRAPS_DECODER 1
#define CONFIG_H261_DECODER 1
#define ENABLE_H261_DECODER 1
#define CONFIG_H263_DECODER 1
#define ENABLE_H263_DECODER 1
#define CONFIG_H263I_DECODER 1
#define ENABLE_H263I_DECODER 1
#define CONFIG_H264_DECODER 1
#define ENABLE_H264_DECODER 1
#define CONFIG_HUFFYUV_DECODER 1
#define ENABLE_HUFFYUV_DECODER 1
#define CONFIG_INDEO2_DECODER 1
#define ENABLE_INDEO2_DECODER 1
#define CONFIG_INDEO3_DECODER 1
#define ENABLE_INDEO3_DECODER 1
#define CONFIG_JPEGLS_DECODER 1
#define ENABLE_JPEGLS_DECODER 1
#define CONFIG_LOCO_DECODER 1
#define ENABLE_LOCO_DECODER 1
#define CONFIG_MJPEG_DECODER 1
#define ENABLE_MJPEG_DECODER 1
#define CONFIG_MJPEGB_DECODER 1
#define ENABLE_MJPEGB_DECODER 1
#define CONFIG_MPEG1VIDEO_DECODER 1
#define ENABLE_MPEG1VIDEO_DECODER 1
#define CONFIG_MPEG2VIDEO_DECODER 1
#define ENABLE_MPEG2VIDEO_DECODER 1
#define CONFIG_MPEG4_DECODER 1
#define ENABLE_MPEG4_DECODER 1
#define CONFIG_MPEGVIDEO_DECODER 1
#define ENABLE_MPEGVIDEO_DECODER 1
#define CONFIG_MSMPEG4V1_DECODER 1
#define ENABLE_MSMPEG4V1_DECODER 1
#define CONFIG_MSMPEG4V2_DECODER 1
#define ENABLE_MSMPEG4V2_DECODER 1
#define CONFIG_MSMPEG4V3_DECODER 1
#define ENABLE_MSMPEG4V3_DECODER 1
#define CONFIG_MSRLE_DECODER 1
#define ENABLE_MSRLE_DECODER 1
#define CONFIG_MSVIDEO1_DECODER 1
#define ENABLE_MSVIDEO1_DECODER 1
#define CONFIG_MSZH_DECODER 1
#define ENABLE_MSZH_DECODER 1
#define CONFIG_PNG_DECODER 1
#define ENABLE_PNG_DECODER 1
#define CONFIG_QPEG_DECODER 1
#define ENABLE_QPEG_DECODER 1
#define CONFIG_QTRLE_DECODER 1
#define ENABLE_QTRLE_DECODER 1
#define CONFIG_RPZA_DECODER 1
#define ENABLE_RPZA_DECODER 1
#define CONFIG_RV10_DECODER 1
#define ENABLE_RV10_DECODER 1
#define CONFIG_RV20_DECODER 1
#define ENABLE_RV20_DECODER 1
#define ENABLE_SNOW_DECODER 0
#define CONFIG_SP5X_DECODER 1
#define ENABLE_SP5X_DECODER 1
#define CONFIG_SVQ1_DECODER 1
#define ENABLE_SVQ1_DECODER 1
#define CONFIG_SVQ3_DECODER 1
#define ENABLE_SVQ3_DECODER 1
#define CONFIG_THEORA_DECODER 1
#define ENABLE_THEORA_DECODER 1
#define CONFIG_TRUEMOTION1_DECODER 1
#define ENABLE_TRUEMOTION1_DECODER 1
#define CONFIG_TRUEMOTION2_DECODER 1
#define ENABLE_TRUEMOTION2_DECODER 1
#define CONFIG_TSCC_DECODER 1
#define ENABLE_TSCC_DECODER 1
#define CONFIG_ULTI_DECODER 1
#define ENABLE_ULTI_DECODER 1
#define CONFIG_VC1_DECODER 1
#define ENABLE_VC1_DECODER 1
#define CONFIG_VCR1_DECODER 1
#define ENABLE_VCR1_DECODER 1
#define CONFIG_VP3_DECODER 1
#define ENABLE_VP3_DECODER 1
#define CONFIG_VP5_DECODER 1
#define ENABLE_VP5_DECODER 1
#define CONFIG_VP6_DECODER 1
#define ENABLE_VP6_DECODER 1
#define CONFIG_VP6A_DECODER 1
#define ENABLE_VP6A_DECODER 1
#define CONFIG_VP6F_DECODER 1
#define ENABLE_VP6F_DECODER 1
#define CONFIG_WMV1_DECODER 1
#define ENABLE_WMV1_DECODER 1
#define CONFIG_WMV2_DECODER 1
#define ENABLE_WMV2_DECODER 1
#define CONFIG_WMV3_DECODER 1
#define ENABLE_WMV3_DECODER 1
#define CONFIG_WNV1_DECODER 1
#define ENABLE_WNV1_DECODER 1
#define CONFIG_XL_DECODER 1
#define ENABLE_XL_DECODER 1
#define CONFIG_ZLIB_DECODER 1
#define ENABLE_ZLIB_DECODER 1
#define CONFIG_ZMBV_DECODER 1
#define ENABLE_ZMBV_DECODER 1
#define CONFIG_ATRAC3_DECODER 1
#define ENABLE_ATRAC3_DECODER 1
#define CONFIG_COOK_DECODER 1
#define ENABLE_COOK_DECODER 1
#define CONFIG_IMC_DECODER 1
#define ENABLE_IMC_DECODER 1
#define CONFIG_MACE3_DECODER 1
#define ENABLE_MACE3_DECODER 1
#define CONFIG_MACE6_DECODER 1
#define ENABLE_MACE6_DECODER 1
#define CONFIG_NELLYMOSER_DECODER 1
#define ENABLE_NELLYMOSER_DECODER 1
#define CONFIG_QDM2_DECODER 1
#define ENABLE_QDM2_DECODER 1
#define CONFIG_RA_144_DECODER 1
#define ENABLE_RA_144_DECODER 1
#define CONFIG_RA_288_DECODER 1
#define ENABLE_RA_288_DECODER 1
#define CONFIG_TRUESPEECH_DECODER 1
#define ENABLE_TRUESPEECH_DECODER 1
#define CONFIG_TTA_DECODER 1
#define ENABLE_TTA_DECODER 1
#define CONFIG_VORBIS_DECODER 1
#define ENABLE_VORBIS_DECODER 1
#define CONFIG_WMAV1_DECODER 1
#define ENABLE_WMAV1_DECODER 1
#define CONFIG_WMAV2_DECODER 1
#define ENABLE_WMAV2_DECODER 1
#define CONFIG_PCM_ALAW_DECODER 1
#define ENABLE_PCM_ALAW_DECODER 1
#define CONFIG_PCM_MULAW_DECODER 1
#define ENABLE_PCM_MULAW_DECODER 1
#define CONFIG_ADPCM_4XM_DECODER 1
#define ENABLE_ADPCM_4XM_DECODER 1
#define CONFIG_ADPCM_ADX_DECODER 1
#define ENABLE_ADPCM_ADX_DECODER 1
#define CONFIG_ADPCM_CT_DECODER 1
#define ENABLE_ADPCM_CT_DECODER 1
#define CONFIG_ADPCM_EA_DECODER 1
#define ENABLE_ADPCM_EA_DECODER 1
#define CONFIG_ADPCM_G726_DECODER 1
#define ENABLE_ADPCM_G726_DECODER 1
#define CONFIG_ADPCM_IMA_AMV_DECODER 1
#define ENABLE_ADPCM_IMA_AMV_DECODER 1
#define CONFIG_ADPCM_IMA_DK3_DECODER 1
#define ENABLE_ADPCM_IMA_DK3_DECODER 1
#define CONFIG_ADPCM_IMA_DK4_DECODER 1
#define ENABLE_ADPCM_IMA_DK4_DECODER 1
#define CONFIG_ADPCM_IMA_QT_DECODER 1
#define ENABLE_ADPCM_IMA_QT_DECODER 1
#define CONFIG_ADPCM_IMA_SMJPEG_DECODER 1
#define ENABLE_ADPCM_IMA_SMJPEG_DECODER 1
#define CONFIG_ADPCM_IMA_WAV_DECODER 1
#define ENABLE_ADPCM_IMA_WAV_DECODER 1
#define CONFIG_ADPCM_IMA_WS_DECODER 1
#define ENABLE_ADPCM_IMA_WS_DECODER 1
#define CONFIG_ADPCM_MS_DECODER 1
#define ENABLE_ADPCM_MS_DECODER 1
#define CONFIG_ADPCM_SBPRO_2_DECODER 1
#define ENABLE_ADPCM_SBPRO_2_DECODER 1
#define CONFIG_ADPCM_SBPRO_3_DECODER 1
#define ENABLE_ADPCM_SBPRO_3_DECODER 1
#define CONFIG_ADPCM_SBPRO_4_DECODER 1
#define ENABLE_ADPCM_SBPRO_4_DECODER 1
#define CONFIG_ADPCM_SWF_DECODER 1
#define ENABLE_ADPCM_SWF_DECODER 1
#define CONFIG_ADPCM_XA_DECODER 1
#define ENABLE_ADPCM_XA_DECODER 1
#define CONFIG_ADPCM_YAMAHA_DECODER 1
#define ENABLE_ADPCM_YAMAHA_DECODER 1
#define CONFIG_DVVIDEO_ENCODER 1
#define ENABLE_DVVIDEO_ENCODER 1
#define CONFIG_FFV1_ENCODER 1
#define ENABLE_FFV1_ENCODER 1
#define CONFIG_FFVHUFF_ENCODER 1
#define ENABLE_FFVHUFF_ENCODER 1
#define CONFIG_FLV_ENCODER 1
#define ENABLE_FLV_ENCODER 1
#define CONFIG_H261_ENCODER 1
#define ENABLE_H261_ENCODER 1
#define CONFIG_H263_ENCODER 1
#define ENABLE_H263_ENCODER 1
#define CONFIG_H263P_ENCODER 1
#define ENABLE_H263P_ENCODER 1
#define ENABLE_H264_ENCODER 0
#define CONFIG_HUFFYUV_ENCODER 1
#define ENABLE_HUFFYUV_ENCODER 1
#define CONFIG_LJPEG_ENCODER 1
#define ENABLE_LJPEG_ENCODER 1
#define CONFIG_MJPEG_ENCODER 1
#define ENABLE_MJPEG_ENCODER 1
#define CONFIG_MPEG1VIDEO_ENCODER 1
#define ENABLE_MPEG1VIDEO_ENCODER 1
#define CONFIG_MPEG2VIDEO_ENCODER 1
#define ENABLE_MPEG2VIDEO_ENCODER 1
#define CONFIG_MPEG4_ENCODER 1
#define ENABLE_MPEG4_ENCODER 1
#define CONFIG_MSMPEG4V1_ENCODER 1
#define ENABLE_MSMPEG4V1_ENCODER 1
#define CONFIG_MSMPEG4V2_ENCODER 1
#define ENABLE_MSMPEG4V2_ENCODER 1
#define CONFIG_MSMPEG4V3_ENCODER 1
#define ENABLE_MSMPEG4V3_ENCODER 1
#define CONFIG_PNG_ENCODER 1
#define ENABLE_PNG_ENCODER 1
#define ENABLE_RV10_ENCODER 0
#define ENABLE_RV20_ENCODER 0
#define ENABLE_SNOW_ENCODER 0
#define CONFIG_WMV1_ENCODER 1
#define ENABLE_WMV1_ENCODER 1
#define CONFIG_WMV2_ENCODER 1
#define ENABLE_WMV2_ENCODER 1
#define CONFIG_AC3_ENCODER 1
#define ENABLE_AC3_ENCODER 1


#define CONFIG_MPEGAUDIO_HP 1
#define ENABLE_MPEGAUDIO_HP 1


#ifndef __GNUC__
  #define EMULATE_FAST_INT
#endif

#define HAVE_7REGS 1
#define HAVE_EBX_AVAILABLE 1

/* CPU specific */
//#define HAVE_FAST_CMOV 1
//#define ENABLE_CMOV 1
