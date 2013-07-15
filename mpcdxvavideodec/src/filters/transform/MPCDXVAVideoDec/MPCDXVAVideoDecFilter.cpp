/*
 * (C) 2007-2013 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "stdafx.h"
#include <math.h>
#include <atlbase.h>
#include <MMReg.h>
#include <evr.h>
#include <vector>
#include <InitGuid.h>
#include "MPCDXVAVideoDecFilter.h"
#include "VideoDecOutputPin.h"

#include "FfmpegContext.h"
extern "C"
{
#include "ffmpeg_dxva/libavcodec/avcodec.h"
}

#include "../../../DSUtil/DSUtil.h"
#include "../../../DSUtil/MediaTypes.h"
#include "../../../DSUtil/SysVersion.h"
#include "../../../DSUtil/WinAPIUtils.h"
#include "../../parser/MpegSplitter/MpegSplitter.h"
#include "moreuuids.h"
#include "DXVADecoderH264.h"

// options names
#define OPT_REGKEY_VideoDec  _T("Software\\Gabest\\Filters\\MPC DXVA Video Decoder")
#define OPT_SECTION_VideoDec _T("Filters\\MPC DXVA Video Decoder")
#define OPT_ThreadNumber     _T("ThreadNumber")
#define OPT_DiscardMode      _T("DiscardMode")
#define OPT_ActiveCodecs     _T("ActiveCodecs")
#define OPT_ARMode           _T("ARMode")
#define OPT_DXVACheck        _T("DXVACheckCompatibility")
#define OPT_DisableDXVA_SD   _T("DisableDXVA_SD")
#define OPT_InterlacedFlag   _T("InterlacedFlag")

#define MAX_SUPPORTED_MODE       5
#define AVRTIMEPERFRAME_VC1_EVO  417083

typedef struct {
    const int   PicEntryNumber;
    const UINT  PreferedConfigBitstream;
    const GUID* Decoder[MAX_SUPPORTED_MODE];
    const WORD  RestrictedMode[MAX_SUPPORTED_MODE];
} DXVA_PARAMS;

typedef struct {
    const CLSID*         clsMinorType;
    const enum AVCodecID nFFCodec;
    const DXVA_PARAMS*   DXVAModes;

    int DXVAModeCount() {
        if (!DXVAModes) {
            return 0;
        }
        for (int i = 0; i < MAX_SUPPORTED_MODE; i++) {
            if (DXVAModes->Decoder[i] == &GUID_NULL) {
                return i;
            }
        }
        return MAX_SUPPORTED_MODE;
    }
} FFMPEG_CODECS;

// DXVA modes supported for H264
DXVA_PARAMS DXVA_H264 = {
    16,     // PicEntryNumber
    2,      // PreferedConfigBitstream
    { &DXVA2_ModeH264_E, &DXVA2_ModeH264_F, &DXVA_Intel_H264_ClearVideo, &GUID_NULL },
    { DXVA_RESTRICTED_MODE_H264_E, 0}
};

DXVA_PARAMS DXVA_H264_VISTA = {
    22,     // PicEntryNumber
    2,      // PreferedConfigBitstream
    { &DXVA2_ModeH264_E, &DXVA2_ModeH264_F, &DXVA_Intel_H264_ClearVideo, &GUID_NULL },
    { DXVA_RESTRICTED_MODE_H264_E, 0}
};

// DXVA modes supported for VC1
DXVA_PARAMS DXVA_VC1 = {
    14,     // PicEntryNumber
    1,      // PreferedConfigBitstream
    { &DXVA2_ModeVC1_D, &GUID_NULL },
    { DXVA_RESTRICTED_MODE_VC1_D, 0}
};

FFMPEG_CODECS ffCodecs[] = {
    // H264/AVC
    { &MEDIASUBTYPE_H264, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_h264, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_X264, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_x264, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_VSSH, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_vssh, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_DAVC, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_davc, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_PAVC, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_pavc, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_AVC1, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_avc1, AV_CODEC_ID_H264,     &DXVA_H264 },
    { &MEDIASUBTYPE_H264_bis, AV_CODEC_ID_H264, &DXVA_H264 },

    // WVC1
    { &MEDIASUBTYPE_WVC1, AV_CODEC_ID_VC1, &DXVA_VC1 },
    { &MEDIASUBTYPE_wvc1, AV_CODEC_ID_VC1, &DXVA_VC1 },
};

/* Important: the order should be exactly the same as in ffCodecs[] */
const AMOVIESETUP_MEDIATYPE CMPCDXVAVideoDecFilter::sudPinTypesIn[] = {
    // H264/AVC
    { &MEDIATYPE_Video, &MEDIASUBTYPE_H264   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_h264   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_X264   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_x264   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_VSSH   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_vssh   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_DAVC   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_davc   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_PAVC   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_pavc   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_AVC1   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_avc1   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_H264_bis },

    // VC1
    { &MEDIATYPE_Video, &MEDIASUBTYPE_WVC1   },
    { &MEDIATYPE_Video, &MEDIASUBTYPE_wvc1   },
};

const int CMPCDXVAVideoDecFilter::sudPinTypesInCount = _countof(CMPCDXVAVideoDecFilter::sudPinTypesIn);

const AMOVIESETUP_MEDIATYPE CMPCDXVAVideoDecFilter::sudPinTypesOut[] = {
    {&MEDIATYPE_Video, &MEDIASUBTYPE_NV12},
    {&MEDIATYPE_Video, &MEDIASUBTYPE_NV24}
};
const int CMPCDXVAVideoDecFilter::sudPinTypesOutCount = _countof(CMPCDXVAVideoDecFilter::sudPinTypesOut);

BOOL CALLBACK EnumFindProcessWnd(HWND hwnd, LPARAM lParam)
{
    DWORD procid = 0;
    TCHAR WindowClass [40];
    GetWindowThreadProcessId(hwnd, &procid);
    GetClassName(hwnd, WindowClass, _countof(WindowClass));

    if (procid == GetCurrentProcessId() && _tcscmp(WindowClass, _T("MediaPlayerClassicW")) == 0) {
        HWND* pWnd = (HWND*)lParam;
        *pWnd = hwnd;
        return FALSE;
    }
    return TRUE;
}

CMPCDXVAVideoDecFilter::CMPCDXVAVideoDecFilter(LPUNKNOWN lpunk, HRESULT* phr)
    : CBaseVideoFilter(MPCDXVAVideoDecName, lpunk, phr, __uuidof(this))
{
    HWND hWnd = nullptr;

    if (SysVersion::IsVistaOrLater()) {
        for (int i = 0; i < _countof(ffCodecs); i++) {
            if (ffCodecs[i].nFFCodec == AV_CODEC_ID_H264) {
                ffCodecs[i].DXVAModes = &DXVA_H264_VISTA;
            }
        }
    }

    if (phr) {
        *phr = S_OK;
    }

    if (m_pOutput)  {
        delete m_pOutput;
    }
    m_pOutput = DEBUG_NEW CVideoDecOutputPin(NAME("CVideoDecOutputPin"), this, phr, L"Output");
    if (!m_pOutput) {
        *phr = E_OUTOFMEMORY;
    }

    m_pAVCodec = nullptr;
    m_pAVCtx = nullptr;
    m_pFrame = nullptr;
    m_nCodecNb = -1;
    m_nCodecId = AV_CODEC_ID_NONE;
    m_bReorderBFrame = true;
    m_DXVADecoderGUID = GUID_NULL;
    m_nActiveCodecs = MPCVD_H264_DXVA | MPCVD_VC1_DXVA;

    m_rtAvrTimePerFrame = 0;
    m_rtLastStart = 0;
    m_nCountEstimated = 0;
    m_rtPrevStop = 0;

    m_nWorkaroundBug = FF_BUG_AUTODETECT;
    m_nErrorConcealment = FF_EC_DEBLOCK | FF_EC_GUESS_MVS;

    m_bDXVACompatible = true;
    m_pFFBuffer = nullptr;
    m_nFFBufferSize = 0;
    m_pAlignedFFBuffer = nullptr;
    m_nAlignedFFBufferSize = 0;
    ResetBuffer();

    m_nWidth = 0;
    m_nHeight = 0;
    m_pSwsContext = nullptr;

    m_bUseDXVA = true;

    m_nDXVAMode = MODE_SOFTWARE;
    m_pDXVADecoder = nullptr;
    m_pVideoOutputFormat = nullptr;
    m_nVideoOutputCount = 0;
    m_hDevice = INVALID_HANDLE_VALUE;

    m_nARMode = 1;
    m_nDXVACheckCompatibility = 1; // skip level check by default
    m_nDXVA_SD = 0;
    m_par.SetSize(1, 1);

    m_interlacedFlag = MPCVC_INTERLACED_AUTO;

    m_bWaitingForKeyFrame = TRUE;
    m_nPosB = 1;
    m_bFrame_repeat_pict = false;
    m_bIsEVO = false;

    CRegKey key;
    if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, OPT_REGKEY_VideoDec, KEY_READ)) {
        DWORD dw;
        if (ERROR_SUCCESS == key.QueryDWORDValue(OPT_ActiveCodecs, dw)) {
            m_nActiveCodecs = dw;
        }
        if (ERROR_SUCCESS == key.QueryDWORDValue(OPT_ARMode, dw)) {
            m_nARMode = dw;
        }
        if (ERROR_SUCCESS == key.QueryDWORDValue(OPT_DXVACheck, dw)) {
            m_nDXVACheckCompatibility = dw;
        }
        if (ERROR_SUCCESS == key.QueryDWORDValue(OPT_DisableDXVA_SD, dw)) {
            m_nDXVA_SD = dw;
        }

        if (ERROR_SUCCESS == key.QueryDWORDValue(OPT_InterlacedFlag, dw)) {
            m_interlacedFlag = (MPCVD_INTERLACED_FLAG)dw;
        }
    }

    if (m_nDXVACheckCompatibility > 3) {
        m_nDXVACheckCompatibility = 1;    // skip level check by default
    }

    ff_avcodec_default_get_buffer = avcodec_default_get_buffer;
    ff_avcodec_default_release_buffer = avcodec_default_release_buffer;
    ff_avcodec_default_reget_buffer = avcodec_default_reget_buffer;

    avcodec_register_all();
    av_log_set_callback(LogLibavcodec);

    EnumWindows(EnumFindProcessWnd, (LPARAM)&hWnd);
    DetectVideoCard(hWnd);

#ifdef _DEBUG
    // Check codec definition table
    int nCodecs = _countof(ffCodecs);
    int nPinTypes = _countof(sudPinTypesIn);
    ASSERT(nCodecs == nPinTypes);
    for (int i = 0; i < nPinTypes; i++) {
        ASSERT(ffCodecs[i].clsMinorType == sudPinTypesIn[i].clsMinorType);
    }
#endif
}

void CMPCDXVAVideoDecFilter::DetectVideoCard(HWND hWnd)
{
    IDirect3D9* pD3D9;
    m_nPCIVendor = 0;
    m_nPCIDevice = 0;
    m_VideoDriverVersion.HighPart = 0;
    m_VideoDriverVersion.LowPart = 0;

    pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (pD3D9) {
        D3DADAPTER_IDENTIFIER9 adapterIdentifier;
        if (pD3D9->GetAdapterIdentifier(GetAdapter(pD3D9, hWnd), 0, &adapterIdentifier) == S_OK) {
            m_nPCIVendor = adapterIdentifier.VendorId;
            m_nPCIDevice = adapterIdentifier.DeviceId;
            m_VideoDriverVersion = adapterIdentifier.DriverVersion;
            m_strDeviceDescription = adapterIdentifier.Description;
            m_strDeviceDescription.AppendFormat(_T(" (%04X:%04X)"), m_nPCIVendor, m_nPCIDevice);
        }
        pD3D9->Release();
    }
}

CMPCDXVAVideoDecFilter::~CMPCDXVAVideoDecFilter()
{
    Cleanup();
}

bool CMPCDXVAVideoDecFilter::IsVideoInterlaced()
{
    // NOT A BUG : always tell DirectShow it's interlaced (progressive flags set in
    // SetTypeSpecificFlags function)
    return true;
};

void CMPCDXVAVideoDecFilter::UpdateFrameTime(REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop, bool b_repeat_pict)
{
    bool m_PullDownFlag = (m_nCodecId == AV_CODEC_ID_VC1 && b_repeat_pict && m_rtAvrTimePerFrame == 333666);
    REFERENCE_TIME m_rtFrameDuration = m_PullDownFlag ? AVRTIMEPERFRAME_VC1_EVO : m_rtAvrTimePerFrame;

    if ((rtStart == _I64_MIN) || (m_PullDownFlag && m_rtPrevStop && (rtStart <= m_rtPrevStop))) {
        rtStart = m_rtLastStart + (REFERENCE_TIME)(m_rtFrameDuration / m_dRate) * m_nCountEstimated;
        m_nCountEstimated++;
    } else {
        m_rtLastStart = rtStart;
        m_nCountEstimated = 1;
    }

    rtStop = rtStart + (REFERENCE_TIME)(m_rtFrameDuration / m_dRate);
}

void CMPCDXVAVideoDecFilter::GetOutputSize(int& w, int& h, int& arx, int& ary, int& RealWidth, int& RealHeight)
{
#if 1
    RealWidth  = m_nWidth;
    RealHeight = m_nHeight;
    w = PictWidthRounded();
    h = PictHeightRounded();
#else
    if (m_nDXVAMode == MODE_SOFTWARE) {
        w = m_nWidth;
        h = m_nHeight;
    } else {
        // DXVA surfaces are multiple of 16 pixels!
        w = PictWidthRounded();
        h = PictHeightRounded();
    }
#endif
}

int CMPCDXVAVideoDecFilter::PictWidth()
{
    return m_nWidth;
}

int CMPCDXVAVideoDecFilter::PictHeight()
{
    return m_nHeight;
}

int CMPCDXVAVideoDecFilter::PictWidthRounded()
{
    // Picture height should be rounded to 16 for DXVA
    return ((m_nWidth + 15) / 16) * 16;
}

int CMPCDXVAVideoDecFilter::PictHeightRounded()
{
    // Picture height should be rounded to 16 for DXVA
    return ((m_nHeight + 15) / 16) * 16;
}

int CMPCDXVAVideoDecFilter::FindCodec(const CMediaType* mtIn)
{
    for (int i = 0; i < _countof(ffCodecs); i++) {
        if (mtIn->subtype == *ffCodecs[i].clsMinorType) {
            bool bCodecActivated = false;
            switch (ffCodecs[i].nFFCodec) {
                case AV_CODEC_ID_H264:
                    m_bUseDXVA = (m_nActiveCodecs & MPCVD_H264_DXVA) != 0;
                    bCodecActivated = m_bUseDXVA;
                    break;
                case AV_CODEC_ID_VC1:
                    m_bUseDXVA = (m_nActiveCodecs & MPCVD_VC1_DXVA) != 0;
                    bCodecActivated = m_bUseDXVA;
                    break;
            }
            return (bCodecActivated ? i : -1);
        }
    }

    return -1;
}

void CMPCDXVAVideoDecFilter::Cleanup()
{
    SAFE_DELETE(m_pDXVADecoder);

    // Release FFmpeg
    if (m_pAVCtx) {
        if (m_pAVCtx->extradata) {
            av_freep(&m_pAVCtx->extradata);
        }
        if (m_pFFBuffer) {
            av_freep(&m_pFFBuffer);
        }
        m_nFFBufferSize = 0;
        if (m_pAlignedFFBuffer) {
            av_freep(&m_pAlignedFFBuffer);
        }
        m_nAlignedFFBufferSize = 0;

        if (m_pAVCtx->codec) {
            avcodec_close(m_pAVCtx);
        }

        av_freep(&m_pAVCtx);
    }

    if (m_pFrame) {
        av_freep(&m_pFrame);
    }

    m_pAVCodec      = nullptr;
    m_pAVCtx        = nullptr;
    m_pFrame        = nullptr;
    m_pFFBuffer     = nullptr;
    m_nFFBufferSize = 0;
    m_nFFBufferPos  = 0;
    m_nFFPicEnd     = INT_MIN;
    m_nCodecNb      = -1;
    m_nCodecId      = AV_CODEC_ID_NONE;
    SAFE_DELETE_ARRAY(m_pVideoOutputFormat);

    // Release DXVA ressources
    if (m_hDevice != INVALID_HANDLE_VALUE) {
        m_pDeviceManager->CloseDeviceHandle(m_hDevice);
        m_hDevice = INVALID_HANDLE_VALUE;
    }

    m_pDeviceManager        = nullptr;
    m_pDecoderService       = nullptr;
    m_pDecoderRenderTarget  = nullptr;
}

void CMPCDXVAVideoDecFilter::CalcAvgTimePerFrame()
{
    CMediaType& mt = m_pInput->CurrentMediaType();
    if (mt.formattype == FORMAT_VideoInfo) {
        m_rtAvrTimePerFrame = ((VIDEOINFOHEADER*)mt.pbFormat)->AvgTimePerFrame;
    } else if (mt.formattype == FORMAT_VideoInfo2) {
        m_rtAvrTimePerFrame = ((VIDEOINFOHEADER2*)mt.pbFormat)->AvgTimePerFrame;
    } else if (mt.formattype == FORMAT_MPEGVideo) {
        m_rtAvrTimePerFrame = ((MPEG1VIDEOINFO*)mt.pbFormat)->hdr.AvgTimePerFrame;
    } else if (mt.formattype == FORMAT_MPEG2Video) {
        m_rtAvrTimePerFrame = ((MPEG2VIDEOINFO*)mt.pbFormat)->hdr.AvgTimePerFrame;
    } else {
        ASSERT(FALSE);
        m_rtAvrTimePerFrame = 1;
    }

    m_rtAvrTimePerFrame = max(1, m_rtAvrTimePerFrame);
}

void CMPCDXVAVideoDecFilter::LogLibavcodec(void* par, int level, const char* fmt, va_list valist)
{
#if defined(_DEBUG) && 0
    if (level <= AV_LOG_VERBOSE) {
        char Msg [500];
        vsnprintf_s(Msg, sizeof(Msg), _TRUNCATE, fmt, valist);
        TRACE(_T("AVLIB : %s\n"), Msg);
    }
#endif
}

void CMPCDXVAVideoDecFilter::OnGetBuffer(AVFrame* pic)
{
    // Callback from FFmpeg to store Ref Time in frame (needed to have correct rtStart after avcodec_decode_video calls)
    //pic->rtStart = m_rtStart;
}

STDMETHODIMP CMPCDXVAVideoDecFilter::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
    return
        QI(IMPCDXVAVideoDecFilter)
        QI(IMPCDXVAVideoDecFilter2)
        QI(ISpecifyPropertyPages)
        QI(ISpecifyPropertyPages2)
        __super::NonDelegatingQueryInterface(riid, ppv);
}



HRESULT CMPCDXVAVideoDecFilter::CheckInputType(const CMediaType* mtIn)
{
    for (int i = 0; i < _countof(sudPinTypesIn); i++) {
        if ((mtIn->majortype == *sudPinTypesIn[i].clsMajorType) &&
                (mtIn->subtype == *sudPinTypesIn[i].clsMinorType)) {
            return S_OK;
        }
    }

    return VFW_E_TYPE_NOT_ACCEPTED;
}

bool CMPCDXVAVideoDecFilter::IsMultiThreadSupported(enum AVCodecID nCodec)
{
    return
        (
            nCodec == AV_CODEC_ID_H264 ||
            nCodec == AV_CODEC_ID_MPEG1VIDEO ||
            nCodec == AV_CODEC_ID_FFV1 ||
            nCodec == AV_CODEC_ID_DVVIDEO ||
            nCodec == AV_CODEC_ID_VP3 ||
            nCodec == AV_CODEC_ID_VP8 ||
            nCodec == AV_CODEC_ID_THEORA ||
            nCodec == AV_CODEC_ID_RV30 ||
            nCodec == AV_CODEC_ID_RV40
        );
}

CString CMPCDXVAVideoDecFilter::GetFileExtension()
{
    CString ext;

    BeginEnumFilters(m_pGraph, pEF, pBF) {
        CComQIPtr<IFileSourceFilter> pFSF = pBF;
        if (pFSF) {
            LPOLESTR pFN = nullptr;
            AM_MEDIA_TYPE mt;
            if (SUCCEEDED(pFSF->GetCurFile(&pFN, &mt)) && pFN && *pFN) {
                ext = CPath(CStringW(pFN)).GetExtension();
                ext.MakeLower();
                CoTaskMemFree(pFN);
            }
            break;
        }
    }
    EndEnumFilters;

    return ext;
}

HRESULT CMPCDXVAVideoDecFilter::SetMediaType(PIN_DIRECTION direction, const CMediaType* pmt)
{
    if (direction == PINDIR_INPUT) {

        int nNewCodec = FindCodec(pmt);

        if (nNewCodec == -1) {
            return VFW_E_TYPE_NOT_ACCEPTED;
        }

        if (nNewCodec != m_nCodecNb) {
            m_nCodecNb = nNewCodec;
            m_nCodecId = ffCodecs[nNewCodec].nFFCodec;

            CLSID ClsidSourceFilter = GetCLSID(m_pInput->GetConnected());
            if ((ClsidSourceFilter == __uuidof(CMpegSourceFilter)) || (ClsidSourceFilter == __uuidof(CMpegSplitterFilter))) {
                if (CComPtr<IBaseFilter> pFilter = GetFilterFromPin(m_pInput->GetConnected())) {
                    if (CComQIPtr<IMpegSplitterFilter> MpegSplitterFilter = pFilter) {
                        m_bIsEVO = (m_nCodecId == AV_CODEC_ID_VC1 && mpeg_ps == MpegSplitterFilter->GetMPEGType());
                    }
                }
            }

            m_bReorderBFrame = true;
            m_pAVCodec = avcodec_find_decoder(m_nCodecId);
            CheckPointer(m_pAVCodec, VFW_E_UNSUPPORTED_VIDEO);

            m_pAVCtx = avcodec_alloc_context3(m_pAVCodec);
            CheckPointer(m_pAVCtx, E_POINTER);

            m_pFrame = avcodec_alloc_frame();
            CheckPointer(m_pFrame, E_POINTER);

            m_h264RandomAccess.flush(m_pAVCtx->thread_count);

            if (pmt->formattype == FORMAT_VideoInfo) {
                VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)pmt->pbFormat;
                m_pAVCtx->width = vih->bmiHeader.biWidth;
                m_pAVCtx->height = abs(vih->bmiHeader.biHeight);
                m_pAVCtx->codec_tag = vih->bmiHeader.biCompression;
                m_pAVCtx->bits_per_coded_sample = vih->bmiHeader.biBitCount;
            } else if (pmt->formattype == FORMAT_VideoInfo2) {
                VIDEOINFOHEADER2* vih2 = (VIDEOINFOHEADER2*)pmt->pbFormat;
                m_pAVCtx->width = vih2->bmiHeader.biWidth;
                m_pAVCtx->height = abs(vih2->bmiHeader.biHeight);
                m_pAVCtx->codec_tag = vih2->bmiHeader.biCompression;
                m_pAVCtx->bits_per_coded_sample = vih2->bmiHeader.biBitCount;
            } else if (pmt->formattype == FORMAT_MPEGVideo) {
                MPEG1VIDEOINFO* mpgv = (MPEG1VIDEOINFO*)pmt->pbFormat;
                m_pAVCtx->width = mpgv->hdr.bmiHeader.biWidth;
                m_pAVCtx->height = abs(mpgv->hdr.bmiHeader.biHeight);
                m_pAVCtx->codec_tag = mpgv->hdr.bmiHeader.biCompression;
                m_pAVCtx->bits_per_coded_sample = mpgv->hdr.bmiHeader.biBitCount;
            } else if (pmt->formattype == FORMAT_MPEG2Video) {
                MPEG2VIDEOINFO* mpg2v = (MPEG2VIDEOINFO*)pmt->pbFormat;
                m_pAVCtx->width = mpg2v->hdr.bmiHeader.biWidth;
                m_pAVCtx->height = abs(mpg2v->hdr.bmiHeader.biHeight);
                m_pAVCtx->codec_tag = mpg2v->hdr.bmiHeader.biCompression;
                m_pAVCtx->bits_per_coded_sample = mpg2v->hdr.bmiHeader.biBitCount;

                if (mpg2v->hdr.bmiHeader.biCompression == 0) {
                    m_pAVCtx->codec_tag = pmt->subtype.Data1;
                } else if ((m_pAVCtx->codec_tag == MAKEFOURCC('a', 'v', 'c', '1')) || (m_pAVCtx->codec_tag == MAKEFOURCC('A', 'V', 'C', '1'))) {
                    m_pAVCtx->nal_length_size = mpg2v->dwFlags;
                    m_bReorderBFrame = (GetFileExtension() == _T(".avi")) ? true : false;
                } else if ((m_pAVCtx->codec_tag == MAKEFOURCC('m', 'p', '4', 'v')) || (m_pAVCtx->codec_tag == MAKEFOURCC('M', 'P', '4', 'V'))) {
                    m_bReorderBFrame = false;
                }
            } else {
                return VFW_E_INVALIDMEDIATYPE;
            }
            m_nWidth = m_pAVCtx->width;
            m_nHeight = m_pAVCtx->height;

            if (m_pAVCtx->codec_tag == MAKEFOURCC('m', 'p', 'g', '2')) {
                m_pAVCtx->codec_tag = MAKEFOURCC('M', 'P', 'E', 'G');
            }

            m_pAVCtx->codec_id          = m_nCodecId;
            m_pAVCtx->workaround_bugs   = m_nWorkaroundBug;
            m_pAVCtx->error_concealment = m_nErrorConcealment;
            m_pAVCtx->err_recognition   = AV_EF_CAREFUL;
            m_pAVCtx->idct_algo         = FF_IDCT_AUTO;
            m_pAVCtx->skip_loop_filter  = AVDISCARD_DEFAULT;

            m_pAVCtx->thread_count = 1;
            m_pAVCtx->thread_type  = 0;

            m_pAVCtx->debug_mv = 0;

            m_pAVCtx->opaque = this;
            m_pAVCtx->get_buffer = get_buffer;

            if (m_nCodecId == AV_CODEC_ID_H264) {
                m_pAVCtx->flags2 |= CODEC_FLAG2_SHOW_ALL;
            }

            AllocExtradata(m_pAVCtx, pmt);
            ConnectTo(m_pAVCtx);
            CalcAvgTimePerFrame();

            if (avcodec_open2(m_pAVCtx, m_pAVCodec, nullptr) < 0) {
                return VFW_E_INVALIDMEDIATYPE;
            }

            // if DXVA is supported in theory the file can still be incompatible
            bool bDXVAAvailableButUnused = IsDXVASupported();
            if (bDXVAAvailableButUnused) {
                do {
                    m_bDXVACompatible = false;

                    if (!DXVACheckFramesize(PictWidth(), PictHeight(), m_nPCIVendor, m_nPCIDevice)) { // check frame size
                        break;
                    }

                    if (m_nCodecId == AV_CODEC_ID_H264) {
                        if (m_nDXVA_SD && PictWidthRounded() < 1280) { // check "Disable DXVA for SD" option
                            break;
                        }
                        int nCompat = FFH264CheckCompatibility(PictWidthRounded(), PictHeightRounded(), m_pAVCtx, (BYTE*)m_pAVCtx->extradata, m_pAVCtx->extradata_size, m_nPCIVendor, m_nPCIDevice, m_VideoDriverVersion);
                        if (nCompat) {
                            if (nCompat == DXVA_HIGH_BIT       ||
                                    m_nDXVACheckCompatibility == 0 || // full check
                                    m_nDXVACheckCompatibility == 1 && nCompat != DXVA_UNSUPPORTED_LEVEL ||   // skip level check
                                    m_nDXVACheckCompatibility == 2 && nCompat != DXVA_TOO_MANY_REF_FRAMES) { // skip reference frame check
                                break;
                            }
                        }
                    }

                    m_bDXVACompatible = true;
                    bDXVAAvailableButUnused = false;
                } while (false);
            }

            if (bDXVAAvailableButUnused) {
                m_bUseDXVA = false;
                avcodec_close(m_pAVCtx);

                return VFW_E_INVALIDMEDIATYPE;
            }

            BuildDXVAOutputFormat();
        }
    }

    HRESULT hr = __super::SetMediaType(direction, pmt);

    if (direction == PINDIR_INPUT) {
        // Compute the expected Pixel AR
        m_par.cx = m_arx * m_h;
        m_par.cy = m_ary * m_w;
        int gcd = GCD(m_par.cx, m_par.cy);
        if (gcd > 1) {
            m_par.cx /= gcd;
            m_par.cy /= gcd;
        }
    }

    return hr;
}

VIDEO_OUTPUT_FORMATS DXVAFormats[] = { // DXVA2
    {&MEDIASUBTYPE_NV12, 1, 12, 'avxd'},
    {&MEDIASUBTYPE_NV12, 1, 12, 'AVXD'},
    {&MEDIASUBTYPE_NV12, 1, 12, 'AVxD'},
    {&MEDIASUBTYPE_NV12, 1, 12, 'AvXD'}
};

VIDEO_OUTPUT_FORMATS SoftwareFormats1[] = { // Software
    {&MEDIASUBTYPE_NV12, 2, 12, '21VN'},
    {&MEDIASUBTYPE_YV12, 3, 12, '21VY'},
    {&MEDIASUBTYPE_YUY2, 1, 16, '2YUY'},
};

VIDEO_OUTPUT_FORMATS SoftwareFormats2[] = { // Software
    {&MEDIASUBTYPE_RGB32, 1, 32, BI_RGB},
};

bool CMPCDXVAVideoDecFilter::IsDXVASupported()
{
    if (m_nCodecNb != -1) {
        // Does the codec suppport DXVA ?
        if (ffCodecs[m_nCodecNb].DXVAModes != nullptr) {
            // Enabled by user ?
            if (m_bUseDXVA) {
                // is the file compatible ?
                if (m_bDXVACompatible) {
                    return true;
                }
            }
        }
    }
    return false;
}

void CMPCDXVAVideoDecFilter::BuildDXVAOutputFormat()
{
    SAFE_DELETE_ARRAY(m_pVideoOutputFormat);

    m_nVideoOutputCount = IsDXVASupported() ? ffCodecs[m_nCodecNb].DXVAModeCount() + _countof(DXVAFormats) : 0;
    m_pVideoOutputFormat = DEBUG_NEW VIDEO_OUTPUT_FORMATS[m_nVideoOutputCount];

    int nPos = 0;
    if (IsDXVASupported()) {
        // Dynamic DXVA media types for DXVA1
        for (nPos = 0; nPos < ffCodecs[m_nCodecNb].DXVAModeCount(); nPos++) {
            m_pVideoOutputFormat[nPos].subtype = ffCodecs[m_nCodecNb].DXVAModes->Decoder[nPos];
            m_pVideoOutputFormat[nPos].biCompression = 'avxd';
            m_pVideoOutputFormat[nPos].biBitCount = 12;
            m_pVideoOutputFormat[nPos].biPlanes = 1;
        }

        // Static list for DXVA2
        memcpy(&m_pVideoOutputFormat[nPos], DXVAFormats, sizeof(DXVAFormats));
        nPos += _countof(DXVAFormats);
    }
}

int CMPCDXVAVideoDecFilter::GetPicEntryNumber()
{
    if (IsDXVASupported()) {
        return ffCodecs[m_nCodecNb].DXVAModes->PicEntryNumber;
    } else {
        return 0;
    }
}

void CMPCDXVAVideoDecFilter::GetOutputFormats(int& nNumber, VIDEO_OUTPUT_FORMATS** ppFormats)
{
    nNumber = m_nVideoOutputCount;
    *ppFormats = m_pVideoOutputFormat;
}

void CMPCDXVAVideoDecFilter::AllocExtradata(AVCodecContext* pAVCtx, const CMediaType* pmt)
{
    // code from LAV ...
    // Process Extradata
    BYTE* extra = nullptr;
    unsigned int extralen = 0;
    getExtraData((const BYTE*)pmt->Format(), pmt->FormatType(), pmt->FormatLength(), nullptr, &extralen);

    BOOL bH264avc = FALSE;
    if (extralen > 0) {
        TRACE(_T("CMPCDXVAVideoDecFilter::AllocExtradata() : processing extradata of %d bytes\n"), extralen);
        // Reconstruct AVC1 extradata format
        if (pmt->formattype == FORMAT_MPEG2Video && (m_pAVCtx->codec_tag == MAKEFOURCC('a', 'v', 'c', '1') || m_pAVCtx->codec_tag == MAKEFOURCC('A', 'V', 'C', '1') || m_pAVCtx->codec_tag == MAKEFOURCC('C', 'C', 'V', '1'))) {
            MPEG2VIDEOINFO* mp2vi = (MPEG2VIDEOINFO*)pmt->Format();
            extralen += 7;
            extra = (uint8_t*)av_mallocz(extralen + FF_INPUT_BUFFER_PADDING_SIZE);
            extra[0] = 1;
            extra[1] = (BYTE)mp2vi->dwProfile;
            extra[2] = 0;
            extra[3] = (BYTE)mp2vi->dwLevel;
            extra[4] = (BYTE)(mp2vi->dwFlags ? mp2vi->dwFlags : 2) - 1;

            // Actually copy the metadata into our new buffer
            unsigned int actual_len;
            getExtraData((const BYTE*)pmt->Format(), pmt->FormatType(), pmt->FormatLength(), extra + 6, &actual_len);

            // Count the number of SPS/PPS in them and set the length
            // We'll put them all into one block and add a second block with 0 elements afterwards
            // The parsing logic does not care what type they are, it just expects 2 blocks.
            BYTE* p = extra + 6, *end = extra + 6 + actual_len;
            BOOL bSPS = FALSE, bPPS = FALSE;
            int count = 0;
            while (p + 1 < end) {
                unsigned len = (((unsigned)p[0] << 8) | p[1]) + 2;
                if (p + len > end) {
                    break;
                }
                if ((p[2] & 0x1F) == 7) {
                    bSPS = TRUE;
                }
                if ((p[2] & 0x1F) == 8) {
                    bPPS = TRUE;
                }
                count++;
                p += len;
            }
            extra[5] = count;
            extra[extralen - 1] = 0;

            bH264avc = TRUE;
            if (!bSPS) {
                TRACE(_T("CMPCDXVAVideoDecFilter::AllocExtradata() : AVC1 extradata doesn't contain a SPS, setting thread_count = 1\n"));
                m_pAVCtx->thread_count = 1;
            }
        } else {
            // Just copy extradata for other formats
            extra = (uint8_t*)av_mallocz(extralen + FF_INPUT_BUFFER_PADDING_SIZE);
            getExtraData((const BYTE*)pmt->Format(), pmt->FormatType(), pmt->FormatLength(), extra, nullptr);
        }
        // Hack to discard invalid MP4 metadata with AnnexB style video
        if (m_nCodecId == AV_CODEC_ID_H264 && !bH264avc && extra[0] == 1) {
            av_freep(&extra);
            extralen = 0;
        }
        m_pAVCtx->extradata = extra;
        m_pAVCtx->extradata_size = (int)extralen;
    }
}

HRESULT CMPCDXVAVideoDecFilter::CompleteConnect(PIN_DIRECTION direction, IPin* pReceivePin)
{
    LOG(_T("CMPCDXVAVideoDecFilter::CompleteConnect"));

    if (direction == PINDIR_INPUT && m_pOutput->IsConnected()) {
        ReconnectOutput(m_nWidth, m_nHeight);
    } else if (direction == PINDIR_OUTPUT) {
        if (IsDXVASupported()) {
            if (m_nDXVAMode == MODE_DXVA1) {
                m_pDXVADecoder->ConfigureDXVA1();
            } else if (SUCCEEDED(ConfigureDXVA2(pReceivePin)) && SUCCEEDED(SetEVRForDXVA2(pReceivePin))) {
                m_nDXVAMode  = MODE_DXVA2;
            }
        }
        if (m_nDXVAMode == MODE_SOFTWARE) {
            return VFW_E_INVALIDMEDIATYPE;
        }

        CLSID ClsidSourceFilter = GetCLSID(m_pInput->GetConnected());
        if ((ClsidSourceFilter == __uuidof(CMpegSourceFilter)) || (ClsidSourceFilter == __uuidof(CMpegSplitterFilter))) {
            m_bReorderBFrame = false;
        }
    }

    return __super::CompleteConnect(direction, pReceivePin);
}

HRESULT CMPCDXVAVideoDecFilter::DecideBufferSize(IMemAllocator* pAllocator, ALLOCATOR_PROPERTIES* pProperties)
{
    if (UseDXVA2()) {
        HRESULT hr;
        ALLOCATOR_PROPERTIES Actual;

        if (m_pInput->IsConnected() == FALSE) {
            return E_UNEXPECTED;
        }

        pProperties->cBuffers = GetPicEntryNumber();

        if (FAILED(hr = pAllocator->SetProperties(pProperties, &Actual))) {
            return hr;
        }

        return pProperties->cBuffers > Actual.cBuffers || pProperties->cbBuffer > Actual.cbBuffer
               ? E_FAIL
               : NOERROR;
    } else {
        return __super::DecideBufferSize(pAllocator, pProperties);
    }
}

HRESULT CMPCDXVAVideoDecFilter::BeginFlush()
{
    return __super::BeginFlush();
}

HRESULT CMPCDXVAVideoDecFilter::EndFlush()
{
    CAutoLock cAutoLock(&m_csReceive);
    return __super::EndFlush();
}

HRESULT CMPCDXVAVideoDecFilter::NewSegment(REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, double dRate)
{
    CAutoLock cAutoLock(&m_csReceive);

    if (m_pAVCtx) {
        avcodec_flush_buffers(m_pAVCtx);
    }

    if (m_pDXVADecoder) {
        m_pDXVADecoder->Flush();
    }

    m_nPosB = 1;
    memset(&m_BFrames, 0, sizeof(m_BFrames));
    m_rtLastStart = 0;
    m_nCountEstimated = 0;
    m_dRate = dRate;

    ResetBuffer();

    m_h264RandomAccess.flush(m_pAVCtx->thread_count);

    m_bWaitingForKeyFrame = TRUE;

    m_rtPrevStop = 0;

    m_rtStart = rtStart;

    return __super::NewSegment(rtStart, rtStop, dRate);
}

HRESULT CMPCDXVAVideoDecFilter::EndOfStream()
{
    CAutoLock cAutoLock(&m_csReceive);

        if (m_nDXVAMode == MODE_DXVA2) { // TODO - need to check DXVA1 on WinXP
            m_pDXVADecoder->EndOfStream();
        }

    return __super::EndOfStream();
}

HRESULT CMPCDXVAVideoDecFilter::BreakConnect(PIN_DIRECTION dir)
{
    if (dir == PINDIR_INPUT) {
        Cleanup();
    }

    return __super::BreakConnect(dir);
}

void CMPCDXVAVideoDecFilter::SetTypeSpecificFlags(IMediaSample* pMS)
{
    if (CComQIPtr<IMediaSample2> pMS2 = pMS) {
        AM_SAMPLE2_PROPERTIES props;
        if (SUCCEEDED(pMS2->GetProperties(sizeof(props), (BYTE*)&props))) {
            props.dwTypeSpecificFlags &= ~0x7f;

            m_nFrameType = PICT_BOTTOM_FIELD;
            if ((m_interlacedFlag == MPCVC_INTERLACED_AUTO && !m_pFrame->interlaced_frame)
                    || m_interlacedFlag == MPCVC_INTERLACED_PROGRESSIVE) {
                props.dwTypeSpecificFlags |= AM_VIDEO_FLAG_WEAVE;
                m_nFrameType = PICT_FRAME;
            } else {
                if ((m_interlacedFlag == MPCVC_INTERLACED_AUTO && m_pFrame->top_field_first)
                        ||  m_interlacedFlag == MPCVC_INTERLACED_TOP_FIELD_FIRST) {
                    props.dwTypeSpecificFlags |= AM_VIDEO_FLAG_FIELD1FIRST;
                    m_nFrameType = PICT_TOP_FIELD;
                }
            }

            switch (m_pFrame->pict_type) {
                case AV_PICTURE_TYPE_I:
                case AV_PICTURE_TYPE_SI:
                    props.dwTypeSpecificFlags |= AM_VIDEO_FLAG_I_SAMPLE;
                    break;
                case AV_PICTURE_TYPE_P:
                case AV_PICTURE_TYPE_SP:
                    props.dwTypeSpecificFlags |= AM_VIDEO_FLAG_P_SAMPLE;
                    break;
                default:
                    props.dwTypeSpecificFlags |= AM_VIDEO_FLAG_B_SAMPLE;
                    break;
            }

            pMS2->SetProperties(sizeof(props), (BYTE*)&props);
        }
    }
}

bool CMPCDXVAVideoDecFilter::FindPicture(int nIndex, int nStartCode)
{
    DWORD dw = 0;

    for (int i = 0; i < m_nFFBufferPos - nIndex; i++) {
        dw = (dw << 8) + m_pFFBuffer[i + nIndex];
        if (i >= 4) {
            if (m_nFFPicEnd == INT_MIN) {
                if ((dw & 0xffffff00) == 0x00000100 &&
                        (dw & 0x000000FF) == (DWORD)nStartCode) {
                    m_nFFPicEnd = i + nIndex - 3;
                }
            } else {
                if ((dw & 0xffffff00) == 0x00000100 &&
                        ((dw & 0x000000FF) == (DWORD)nStartCode || (dw & 0x000000FF) == 0xB3)) {
                    m_nFFPicEnd = i + nIndex - 3;
                    return true;
                }
            }
        }

    }

    return false;
}

void CMPCDXVAVideoDecFilter::ResetBuffer()
{
    m_nFFBufferPos = 0;
    m_nFFPicEnd = INT_MIN;

    for (int i = 0; i < MAX_BUFF_TIME; i++) {
        m_FFBufferTime[i].nBuffPos = INT_MIN;
        m_FFBufferTime[i].rtStart = _I64_MIN;
        m_FFBufferTime[i].rtStop = _I64_MIN;
    }
}

void CMPCDXVAVideoDecFilter::PushBufferTime(int nPos, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop)
{
    for (int i = 0; i < MAX_BUFF_TIME; i++) {
        if (m_FFBufferTime[i].nBuffPos == INT_MIN) {
            m_FFBufferTime[i].nBuffPos = nPos;
            m_FFBufferTime[i].rtStart = rtStart;
            m_FFBufferTime[i].rtStop = rtStop;
            break;
        }
    }
}

void CMPCDXVAVideoDecFilter::PopBufferTime(int nPos)
{
    int nDestPos = 0;
    int i = 0;

    // Shift buffer time list
    while (i < MAX_BUFF_TIME && m_FFBufferTime[i].nBuffPos != INT_MIN) {
        if (m_FFBufferTime[i].nBuffPos >= nPos) {
            m_FFBufferTime[nDestPos].nBuffPos = m_FFBufferTime[i].nBuffPos - nPos;
            m_FFBufferTime[nDestPos].rtStart = m_FFBufferTime[i].rtStart;
            m_FFBufferTime[nDestPos].rtStop = m_FFBufferTime[i].rtStop;
            nDestPos++;
        }
        i++;
    }

    // Free unused slots
    for (i = nDestPos; i < MAX_BUFF_TIME; i++) {
        m_FFBufferTime[i].nBuffPos = INT_MIN;
        m_FFBufferTime[i].rtStart = _I64_MIN;
        m_FFBufferTime[i].rtStop = _I64_MIN;
    }
}

bool CMPCDXVAVideoDecFilter::AppendBuffer(BYTE* pDataIn, int nSize, REFERENCE_TIME rtStart, REFERENCE_TIME rtStop)
{
    if (rtStart != _I64_MIN) {
        PushBufferTime(m_nFFBufferPos, rtStart, rtStop);
    }

    if (m_nFFBufferPos + nSize + FF_INPUT_BUFFER_PADDING_SIZE > m_nFFBufferSize) {
        m_nFFBufferSize = m_nFFBufferPos + nSize + FF_INPUT_BUFFER_PADDING_SIZE;
        m_pFFBuffer = (BYTE*)av_realloc(m_pFFBuffer, m_nFFBufferSize);
    }

    memcpy(m_pFFBuffer + m_nFFBufferPos, pDataIn, nSize);

    m_nFFBufferPos += nSize;

    return true;
}

void CMPCDXVAVideoDecFilter::ShrinkBuffer()
{
    int nRemaining = m_nFFBufferPos - m_nFFPicEnd;

    ASSERT(m_nFFPicEnd != INT_MIN);

    PopBufferTime(m_nFFPicEnd);
    memcpy(m_pFFBuffer, m_pFFBuffer + m_nFFPicEnd, nRemaining);
    m_nFFBufferPos = nRemaining;

    m_nFFPicEnd = (m_pFFBuffer[3] == 0x00) ?  0 : INT_MIN;
}

HRESULT CMPCDXVAVideoDecFilter::Transform(IMediaSample* pIn)
{
    CAutoLock cAutoLock(&m_csReceive);
    HRESULT hr;
    BYTE* pDataIn;
    int nSize;
    REFERENCE_TIME rtStart = _I64_MIN;
    REFERENCE_TIME rtStop  = _I64_MIN;

    if (FAILED(hr = pIn->GetPointer(&pDataIn))) {
        return hr;
    }

    nSize = pIn->GetActualDataLength();
    hr = pIn->GetTime(&rtStart, &rtStop);

    if (FAILED(hr)) {
        rtStart = rtStop = _I64_MIN;
    }

    if (m_nDXVAMode == MODE_SOFTWARE || (m_nCodecId == AV_CODEC_ID_VC1 && !m_bIsEVO)) {
        UpdateFrameTime(rtStart, rtStop, m_bFrame_repeat_pict);
    }

    m_pAVCtx->reordered_opaque  = rtStart;
    m_pAVCtx->reordered_opaque2 = rtStop;

    if (m_pAVCtx->has_b_frames) {
        m_BFrames[m_nPosB].rtStart = rtStart;
        m_BFrames[m_nPosB].rtStop  = rtStop;
        m_nPosB = 1 - m_nPosB;
    }

    switch (m_nDXVAMode) {
        case MODE_DXVA1:
        case MODE_DXVA2:
            CheckPointer(m_pDXVADecoder, E_UNEXPECTED);
            UpdateAspectRatio();

            // Change aspect ratio for DXVA1
            if ((m_nDXVAMode == MODE_DXVA1) &&
                    ReconnectOutput(PictWidthRounded(), PictHeightRounded(), true, PictWidth(), PictHeight()) == S_OK) {
                m_pDXVADecoder->ConfigureDXVA1();
            }

            if (m_pAVCtx->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
                AppendBuffer(pDataIn, nSize, rtStart, rtStop);
                hr = S_OK;

                while (FindPicture(max(m_nFFBufferPos - nSize - 4, 0), 0x00)) {
                    if (m_FFBufferTime[0].nBuffPos != INT_MIN && m_FFBufferTime[0].nBuffPos < m_nFFPicEnd) {
                        rtStart = m_FFBufferTime[0].rtStart;
                        rtStop  = m_FFBufferTime[0].rtStop;
                    } else {
                        rtStart = rtStop = _I64_MIN;
                    }
                    hr = m_pDXVADecoder->DecodeFrame(m_pFFBuffer, m_nFFPicEnd, rtStart, rtStop);
                    ShrinkBuffer();
                }
            } else {
                hr = m_pDXVADecoder->DecodeFrame(pDataIn, nSize, rtStart, rtStop);
            }
            break;
        default:
            ASSERT(FALSE);
            hr = E_UNEXPECTED;
    }

    return hr;
}

void CMPCDXVAVideoDecFilter::UpdateAspectRatio()
{
    if (m_nARMode && m_pAVCtx && (m_pAVCtx->sample_aspect_ratio.num > 0) && (m_pAVCtx->sample_aspect_ratio.den > 0)) {
        CSize PAR(m_pAVCtx->sample_aspect_ratio.num, m_pAVCtx->sample_aspect_ratio.den);
        if (m_par != PAR) {
            m_par = PAR;
            CSize aspect(m_nWidth * PAR.cx, m_nHeight * PAR.cy);
            int gcd = GCD(aspect.cx, aspect.cy);
            if (gcd > 1) {
                aspect.cx /= gcd;
                aspect.cy /= gcd;
            }
            SetAspect(aspect);
        }
    }
}

void CMPCDXVAVideoDecFilter::ReorderBFrames(REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop)
{
    // Re-order B-frames if needed
    if (m_pAVCtx->has_b_frames && m_bReorderBFrame) {
        rtStart = m_BFrames [m_nPosB].rtStart;
        rtStop  = m_BFrames [m_nPosB].rtStop;
    }
}

void CMPCDXVAVideoDecFilter::FillInVideoDescription(DXVA2_VideoDesc* pDesc)
{
    memset(pDesc, 0, sizeof(DXVA2_VideoDesc));
    pDesc->SampleWidth = PictWidthRounded();
    pDesc->SampleHeight = PictHeightRounded();
    pDesc->Format = D3DFMT_A8R8G8B8;
    pDesc->UABProtectionLevel = 1;
}

BOOL CMPCDXVAVideoDecFilter::IsSupportedDecoderMode(const GUID& mode)
{
    if (IsDXVASupported()) {
        for (int i = 0; i < MAX_SUPPORTED_MODE; i++) {
            if (*ffCodecs[m_nCodecNb].DXVAModes->Decoder[i] == GUID_NULL) {
                break;
            } else if (*ffCodecs[m_nCodecNb].DXVAModes->Decoder[i] == mode) {
                return true;
            }
        }
    }

    return false;
}

BOOL CMPCDXVAVideoDecFilter::IsSupportedDecoderConfig(const D3DFORMAT nD3DFormat, const DXVA2_ConfigPictureDecode& config, bool& bIsPrefered)
{
    bool bRet = false;

    bRet = (nD3DFormat == MAKEFOURCC('N', 'V', '1', '2') || nD3DFormat == MAKEFOURCC('I', 'M', 'C', '3'));

    bIsPrefered = (config.ConfigBitstreamRaw == ffCodecs[m_nCodecNb].DXVAModes->PreferedConfigBitstream);
    LOG(_T("IsSupportedDecoderConfig  0x%08x  %d"), nD3DFormat, bRet);
    return bRet;
}

HRESULT CMPCDXVAVideoDecFilter::FindDXVA2DecoderConfiguration(IDirectXVideoDecoderService* pDecoderService,
        const GUID& guidDecoder,
        DXVA2_ConfigPictureDecode* pSelectedConfig,
        BOOL* pbFoundDXVA2Configuration)
{
    HRESULT hr = S_OK;
    UINT cFormats = 0;
    UINT cConfigurations = 0;
    bool bIsPrefered = false;

    D3DFORMAT* pFormats = nullptr;                 // size = cFormats
    DXVA2_ConfigPictureDecode* pConfig = nullptr;  // size = cConfigurations

    // Find the valid render target formats for this decoder GUID.
    hr = pDecoderService->GetDecoderRenderTargets(guidDecoder, &cFormats, &pFormats);
    LOG(_T("GetDecoderRenderTargets => %d"), cFormats);

    if (SUCCEEDED(hr)) {
        // Look for a format that matches our output format.
        for (UINT iFormat = 0; iFormat < cFormats;  iFormat++) {
            LOG(_T("Try to negociate => 0x%08x"), pFormats[iFormat]);

            // Fill in the video description. Set the width, height, format, and frame rate.
            FillInVideoDescription(&m_VideoDesc); // Private helper function.
            m_VideoDesc.Format = pFormats[iFormat];

            // Get the available configurations.
            hr = pDecoderService->GetDecoderConfigurations(guidDecoder, &m_VideoDesc, nullptr, &cConfigurations, &pConfig);

            if (FAILED(hr)) {
                continue;
            }

            // Find a supported configuration.
            for (UINT iConfig = 0; iConfig < cConfigurations; iConfig++) {
                if (IsSupportedDecoderConfig(pFormats[iFormat], pConfig[iConfig], bIsPrefered)) {
                    // This configuration is good.
                    if (bIsPrefered || !*pbFoundDXVA2Configuration) {
                        *pbFoundDXVA2Configuration = TRUE;
                        *pSelectedConfig = pConfig[iConfig];
                    }

                    if (bIsPrefered) {
                        break;
                    }
                }
            }

            CoTaskMemFree(pConfig);
        } // End of formats loop.
    }

    CoTaskMemFree(pFormats);

    // Note: It is possible to return S_OK without finding a configuration.
    return hr;
}

HRESULT CMPCDXVAVideoDecFilter::ConfigureDXVA2(IPin* pPin)
{
    HRESULT hr = S_OK;
    UINT cDecoderGuids = 0;
    BOOL bFoundDXVA2Configuration = FALSE;
    BOOL bHasIntelGuid = FALSE;
    GUID guidDecoder = GUID_NULL;

    DXVA2_ConfigPictureDecode config;
    ZeroMemory(&config, sizeof(config));

    CComPtr<IMFGetService> pGetService;
    CComPtr<IDirect3DDeviceManager9> pDeviceManager;
    CComPtr<IDirectXVideoDecoderService> pDecoderService;
    GUID* pDecoderGuids = nullptr;
    HANDLE hDevice = INVALID_HANDLE_VALUE;

    // Query the pin for IMFGetService.
    hr = pPin->QueryInterface(__uuidof(IMFGetService), (void**)&pGetService);

    // Get the Direct3D device manager.
    if (SUCCEEDED(hr)) {
        hr = pGetService->GetService(
                 MR_VIDEO_ACCELERATION_SERVICE,
                 __uuidof(IDirect3DDeviceManager9),
                 (void**)&pDeviceManager);
    }

    // Open a new device handle.
    if (SUCCEEDED(hr)) {
        hr = pDeviceManager->OpenDeviceHandle(&hDevice);
    }

    // Get the video decoder service.
    if (SUCCEEDED(hr)) {
        hr = pDeviceManager->GetVideoService(
                 hDevice,
                 __uuidof(IDirectXVideoDecoderService),
                 (void**)&pDecoderService);
    }

    // Get the decoder GUIDs.
    if (SUCCEEDED(hr)) {
        hr = pDecoderService->GetDecoderDeviceGuids(&cDecoderGuids, &pDecoderGuids);
    }

    if (SUCCEEDED(hr)) {

        // Intel patch for Ivy Bridge and Sandy Bridge
        if (m_nPCIVendor == PCIV_Intel) {
            for (UINT iCnt = 0; iCnt < cDecoderGuids; iCnt++) {
                if (pDecoderGuids[iCnt] == DXVA_Intel_H264_ClearVideo) {
                    bHasIntelGuid = TRUE;
                }
            }
        }
        // Look for the decoder GUIDs we want.
        for (UINT iGuid = 0; iGuid < cDecoderGuids; iGuid++) {
            // Do we support this mode?
            if (!IsSupportedDecoderMode(pDecoderGuids[iGuid])) {
                continue;
            }

            // Find a configuration that we support.
            hr = FindDXVA2DecoderConfiguration(pDecoderService, pDecoderGuids[iGuid], &config, &bFoundDXVA2Configuration);

            if (FAILED(hr)) {
                break;
            }

            // Patch for the Sandy Bridge (prevent crash on Mode_E, fixme later)
            if (m_nPCIVendor == PCIV_Intel && pDecoderGuids[iGuid] == DXVA2_ModeH264_E && bHasIntelGuid) {
                continue;
            }

            if (bFoundDXVA2Configuration) {
                // Found a good configuration. Save the GUID.
                guidDecoder = pDecoderGuids[iGuid];
                if (!bHasIntelGuid) {
                    break;
                }
            }
        }
    }

    if (pDecoderGuids) {
        CoTaskMemFree(pDecoderGuids);
    }
    if (!bFoundDXVA2Configuration) {
        hr = E_FAIL; // Unable to find a configuration.
    }

    if (SUCCEEDED(hr)) {
        // Store the things we will need later.
        m_pDeviceManager = pDeviceManager;
        m_pDecoderService = pDecoderService;

        m_DXVA2Config = config;
        m_DXVADecoderGUID = guidDecoder;
        m_hDevice = hDevice;
    }

    if (FAILED(hr)) {
        if (hDevice != INVALID_HANDLE_VALUE) {
            pDeviceManager->CloseDeviceHandle(hDevice);
        }
    }

    return hr;
}

HRESULT CMPCDXVAVideoDecFilter::SetEVRForDXVA2(IPin* pPin)
{
    HRESULT hr = S_OK;

    CComPtr<IMFGetService> pGetService;
    CComPtr<IDirectXVideoMemoryConfiguration> pVideoConfig;
    CComPtr<IMFVideoDisplayControl> pVdc;

    // Query the pin for IMFGetService.
    hr = pPin->QueryInterface(__uuidof(IMFGetService), (void**)&pGetService);

    // Get the IDirectXVideoMemoryConfiguration interface.
    if (SUCCEEDED(hr)) {
        hr = pGetService->GetService(
                 MR_VIDEO_ACCELERATION_SERVICE,
                 __uuidof(IDirectXVideoMemoryConfiguration),
                 (void**)&pVideoConfig);

        if (SUCCEEDED(pGetService->GetService(MR_VIDEO_RENDER_SERVICE, __uuidof(IMFVideoDisplayControl), (void**)&pVdc))) {
            HWND    hWnd;
            if (SUCCEEDED(pVdc->GetVideoWindow(&hWnd))) {
                DetectVideoCard(hWnd);
            }
        }
    }

    // Notify the EVR.
    if (SUCCEEDED(hr)) {
        DXVA2_SurfaceType surfaceType;

        for (DWORD iTypeIndex = 0; ; iTypeIndex++) {
            hr = pVideoConfig->GetAvailableSurfaceTypeByIndex(iTypeIndex, &surfaceType);

            if (FAILED(hr)) {
                break;
            }

            if (surfaceType == DXVA2_SurfaceType_DecoderRenderTarget) {
                hr = pVideoConfig->SetSurfaceType(DXVA2_SurfaceType_DecoderRenderTarget);
                break;
            }
        }
    }

    return hr;
}

HRESULT CMPCDXVAVideoDecFilter::CreateDXVA2Decoder(UINT nNumRenderTargets, IDirect3DSurface9** pDecoderRenderTargets)
{
    HRESULT hr;
    CComPtr<IDirectXVideoDecoder> pDirectXVideoDec;

    m_pDecoderRenderTarget = nullptr;

    if (m_pDXVADecoder) {
        m_pDXVADecoder->SetDirectXVideoDec(nullptr);
    }

    hr = m_pDecoderService->CreateVideoDecoder(m_DXVADecoderGUID, &m_VideoDesc, &m_DXVA2Config,
            pDecoderRenderTargets, nNumRenderTargets, &pDirectXVideoDec);

    if (SUCCEEDED(hr)) {
        #if 0 // this may be a cause of crashes
        if (m_nPCIVendor == PCIV_Intel) {
            // We need to recreate the dxva decoder after "stop" on Intel HD Graphics
            SAFE_DELETE(m_pDXVADecoder);
        }
        #endif
        if (!m_pDXVADecoder) {
            m_pDXVADecoder = CDXVADecoder::CreateDecoder(this, pDirectXVideoDec, &m_DXVADecoderGUID, GetPicEntryNumber(), &m_DXVA2Config);
            if (m_pDXVADecoder) {
                m_pDXVADecoder->SetExtraData((BYTE*)m_pAVCtx->extradata, m_pAVCtx->extradata_size);
            }
        }

        if (m_pDXVADecoder) {
            m_pDXVADecoder->SetDirectXVideoDec(pDirectXVideoDec);
        } else {
            return E_FAIL;
        }
    }

    return hr;
}

HRESULT CMPCDXVAVideoDecFilter::FindDXVA1DecoderConfiguration(IAMVideoAccelerator* pAMVideoAccelerator, const GUID* guidDecoder, DDPIXELFORMAT* pPixelFormat)
{
    HRESULT hr = E_FAIL;
    DWORD dwFormats = 0;
    DDPIXELFORMAT* pPixelFormats = nullptr;


    pAMVideoAccelerator->GetUncompFormatsSupported(guidDecoder, &dwFormats, nullptr);
    if (dwFormats > 0) {
        // Find the valid render target formats for this decoder GUID.
        pPixelFormats = DEBUG_NEW DDPIXELFORMAT[dwFormats];
        hr = pAMVideoAccelerator->GetUncompFormatsSupported(guidDecoder, &dwFormats, pPixelFormats);
        if (SUCCEEDED(hr)) {
            // Look for a format that matches our output format.
            for (DWORD iFormat = 0; iFormat < dwFormats; iFormat++) {
                if (pPixelFormats[iFormat].dwFourCC == MAKEFOURCC('N', 'V', '1', '2')) {
                    memcpy(pPixelFormat, &pPixelFormats[iFormat], sizeof(DDPIXELFORMAT));
                    SAFE_DELETE_ARRAY(pPixelFormats);
                    return S_OK;
                }
            }

            SAFE_DELETE_ARRAY(pPixelFormats);
            hr = E_FAIL;
        }
    }

    return hr;
}

HRESULT CMPCDXVAVideoDecFilter::CheckDXVA1Decoder(const GUID* pGuid)
{
    if (m_nCodecNb != -1) {
        for (int i = 0; i < MAX_SUPPORTED_MODE; i++) {
            if (*ffCodecs[m_nCodecNb].DXVAModes->Decoder[i] == *pGuid) {
                return S_OK;
            }
        }
    }

    return E_INVALIDARG;
}

void CMPCDXVAVideoDecFilter::SetDXVA1Params(const GUID* pGuid, DDPIXELFORMAT* pPixelFormat)
{
    m_DXVADecoderGUID = *pGuid;
    memcpy(&m_PixelFormat, pPixelFormat, sizeof(DDPIXELFORMAT));
}

WORD CMPCDXVAVideoDecFilter::GetDXVA1RestrictedMode()
{
    if (m_nCodecNb != -1) {
        for (int i = 0; i < MAX_SUPPORTED_MODE; i++) {
            if (*ffCodecs[m_nCodecNb].DXVAModes->Decoder[i] == m_DXVADecoderGUID) {
                return ffCodecs[m_nCodecNb].DXVAModes->RestrictedMode [i];
            }
        }
    }

    return DXVA_RESTRICTED_MODE_UNRESTRICTED;
}

HRESULT CMPCDXVAVideoDecFilter::CreateDXVA1Decoder(IAMVideoAccelerator*  pAMVideoAccelerator, const GUID* pDecoderGuid, DWORD dwSurfaceCount)
{
    if (m_pDXVADecoder && m_DXVADecoderGUID == *pDecoderGuid) {
        return S_OK;
    }
    SAFE_DELETE(m_pDXVADecoder);

    if (!m_bUseDXVA) {
        return E_FAIL;
    }

    m_nDXVAMode = MODE_DXVA1;
    m_DXVADecoderGUID = *pDecoderGuid;
    m_pDXVADecoder = CDXVADecoder::CreateDecoder(this, pAMVideoAccelerator, &m_DXVADecoderGUID, dwSurfaceCount);
    if (m_pDXVADecoder) {
        m_pDXVADecoder->SetExtraData((BYTE*)m_pAVCtx->extradata, m_pAVCtx->extradata_size);
    }

    return S_OK;
}

// ISpecifyPropertyPages2

STDMETHODIMP CMPCDXVAVideoDecFilter::GetPages(CAUUID* pPages)
{
    CheckPointer(pPages, E_POINTER);

    HRESULT hr = S_OK;

    pPages->cElems = 2;
    pPages->pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID) * pPages->cElems);
    if (pPages->pElems != nullptr) {
        pPages->pElems[0] = __uuidof(CMPCDXVAVideoDecSettingsWnd);
        if (pPages->cElems > 1) {
            pPages->pElems[1] = __uuidof(CMPCDXVAVideoDecCodecWnd);
        }
    } else {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

STDMETHODIMP CMPCDXVAVideoDecFilter::CreatePage(const GUID& guid, IPropertyPage** ppPage)
{
    CheckPointer(ppPage, E_POINTER);

    if (*ppPage != nullptr) {
        return E_INVALIDARG;
    }

    HRESULT hr;

    if (guid == __uuidof(CMPCDXVAVideoDecSettingsWnd)) {
        (*ppPage = DEBUG_NEW CInternalPropertyPageTempl<CMPCDXVAVideoDecSettingsWnd>(nullptr, &hr))->AddRef();
    } else if (guid == __uuidof(CMPCDXVAVideoDecCodecWnd)) {
        (*ppPage = DEBUG_NEW CInternalPropertyPageTempl<CMPCDXVAVideoDecCodecWnd>(nullptr, &hr))->AddRef();
    }

    return *ppPage ? S_OK : E_FAIL;
}

void CMPCDXVAVideoDecFilter::SetFrameType(FF_FIELD_TYPE nFrameType)
{
    m_nFrameType = nFrameType;
}

// IFFmpegDecFilter
STDMETHODIMP CMPCDXVAVideoDecFilter::Apply()
{
    CRegKey key;
    if (ERROR_SUCCESS == key.Create(HKEY_CURRENT_USER, OPT_REGKEY_VideoDec)) {
        key.SetDWORDValue(OPT_ActiveCodecs, m_nActiveCodecs);
        key.SetDWORDValue(OPT_ARMode, m_nARMode);
        key.SetDWORDValue(OPT_DXVACheck, m_nDXVACheckCompatibility);
        key.SetDWORDValue(OPT_DisableDXVA_SD, m_nDXVA_SD);
        key.SetDWORDValue(OPT_InterlacedFlag, m_interlacedFlag);
    }

    return S_OK;
}

// === IMPCDXVAVideoDecFilter

STDMETHODIMP_(GUID*) CMPCDXVAVideoDecFilter::GetDXVADecoderGuid()
{
    if (m_pGraph == nullptr) {
        return nullptr;
    } else {
        return &m_DXVADecoderGUID;
    }
}

STDMETHODIMP CMPCDXVAVideoDecFilter::SetActiveCodecs(MPC_VIDEO_CODEC nValue)
{
    CAutoLock cAutoLock(&m_csProps);
    m_nActiveCodecs = (int)nValue;
    return S_OK;
}

STDMETHODIMP_(MPC_VIDEO_CODEC) CMPCDXVAVideoDecFilter::GetActiveCodecs()
{
    CAutoLock cAutoLock(&m_csProps);
    return (MPC_VIDEO_CODEC)m_nActiveCodecs;
}

STDMETHODIMP_(LPCTSTR) CMPCDXVAVideoDecFilter::GetVideoCardDescription()
{
    CAutoLock cAutoLock(&m_csProps);
    return m_strDeviceDescription;
}

STDMETHODIMP CMPCDXVAVideoDecFilter::SetARMode(int nValue)
{
    CAutoLock cAutoLock(&m_csProps);
    m_nARMode = nValue;
    return S_OK;
}
STDMETHODIMP_(int) CMPCDXVAVideoDecFilter::GetARMode()
{
    CAutoLock cAutoLock(&m_csProps);
    return m_nARMode;
}
STDMETHODIMP CMPCDXVAVideoDecFilter::SetDXVACheckCompatibility(int nValue)
{
    CAutoLock cAutoLock(&m_csProps);
    m_nDXVACheckCompatibility = nValue;
    return S_OK;
}

STDMETHODIMP_(int) CMPCDXVAVideoDecFilter::GetDXVACheckCompatibility()
{
    CAutoLock cAutoLock(&m_csProps);
    return m_nDXVACheckCompatibility;
}

STDMETHODIMP CMPCDXVAVideoDecFilter::SetDXVA_SD(int nValue)
{
    CAutoLock cAutoLock(&m_csProps);
    m_nDXVA_SD = nValue;
    return S_OK;
}

STDMETHODIMP_(int) CMPCDXVAVideoDecFilter::GetDXVA_SD()
{
    CAutoLock cAutoLock(&m_csProps);
    return m_nDXVA_SD;
}

// === IMPCDXVAVideoDecFilter2
STDMETHODIMP_(int) CMPCDXVAVideoDecFilter::GetFrameType()
{
    CAutoLock cAutoLock(&m_csProps);
    return m_nFrameType;
}

STDMETHODIMP CMPCDXVAVideoDecFilter::SetInterlacedFlag(MPCVD_INTERLACED_FLAG interlacedFlag)
{
    CAutoLock cAutoLock(&m_csProps);
    m_interlacedFlag = interlacedFlag;
    return S_OK;
}

STDMETHODIMP_(MPCVD_INTERLACED_FLAG) CMPCDXVAVideoDecFilter::GetInterlacedFlag()
{
    CAutoLock cAutoLock(&m_csProps);
    return m_interlacedFlag;
}
