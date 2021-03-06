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

#pragma once

#include <d3dx9.h>
#include <videoacc.h>   // DXVA1
#include <dxva.h>
#include <dxva2api.h>   // DXVA2
#include "../BaseVideoFilter/BaseVideoFilter.h"

#include "IMPCDXVAVideoDecFilter.h"
#include "MPCDXVAVideoDecSettingsWnd.h"
//#include "../../../DeCSS/DeCSSInputPin.h"
#include "DXVADecoder.h"
#include "TlibavcodecExt.h"

#include "H264RandomAccess.h"
#include <atlpath.h>

// TODO: remove this when it's fixed in MSVC
// Work around warning C4005: 'XXXX' : macro redefinition
#pragma warning(push)
#pragma warning(disable: 4005)
#include <stdint.h>
#pragma warning(pop)


#define MPCDXVAVideoDecName L"MPC DXVA Video Decoder"

#define MAX_BUFF_TIME   20

#define CHECK_HR_TRACE(x)                   \
    hr = ##x;                               \
    if (FAILED(hr)) {                       \
        TRACE(_T("Error : 0x%08x\n"), hr);  \
        ASSERT(hr == VFW_E_NOT_COMMITTED);  \
        return hr;                          \
    }

struct AVCodec;
struct AVCodecContext;
struct AVFrame;
struct SwsContext;

typedef enum {
    MODE_SOFTWARE,
    MODE_DXVA1,
    MODE_DXVA2
} DXVA_MODE;

typedef struct {
    REFERENCE_TIME rtStart;
    REFERENCE_TIME rtStop;
} B_FRAME;

typedef struct {
    REFERENCE_TIME rtStart;
    REFERENCE_TIME rtStop;
    int nBuffPos;
} BUFFER_TIME;

class __declspec(uuid("04490B85-C276-480A-8D17-835E4429067D"))
    CMPCDXVAVideoDecFilter
    : public CBaseVideoFilter
    , public TlibavcodecExt
    , public ISpecifyPropertyPages2
    , public IMPCDXVAVideoDecFilter2
{
protected:

    // === FFmpeg callbacks
    static void LogLibavcodec(void* par, int level, const char* fmt, va_list valist);
    virtual void OnGetBuffer(AVFrame* pic);

    friend class CVideoDecDXVAAllocator;

    CCritSec m_csProps;

    CAutoPtr<bool> m_DXVAFilters;

    // === Persistants parameters (registry)
    bool m_bDXVACompatible;
    int m_nActiveCodecs;
    int m_nARMode;
    int m_nDXVACheckCompatibility;
    int m_nDXVA_SD;
    MPCVD_INTERLACED_FLAG m_interlacedFlag;

    FF_FIELD_TYPE m_nFrameType;

    // === FFmpeg variables
    AVCodec* m_pAVCodec;
    AVCodecContext* m_pAVCtx;
    AVFrame* m_pFrame;
    int m_nCodecNb;
    enum AVCodecID m_nCodecId;
    int m_nWorkaroundBug;
    int m_nErrorConcealment;
    REFERENCE_TIME m_rtAvrTimePerFrame;
    bool m_bReorderBFrame;
    B_FRAME m_BFrames[2];
    int m_nPosB;
    int m_nWidth;       // Frame width given to input pin
    int m_nHeight;      // Frame height given to input pin

    bool m_bIsEVO;

    // Buffer management for truncated stream (store stream chunks & reference time sent by splitter)
    BYTE* m_pFFBuffer;
    int m_nFFBufferSize;
    BYTE* m_pAlignedFFBuffer;
    int m_nAlignedFFBufferSize;

    int m_nFFBufferPos;
    int m_nFFPicEnd;
    BUFFER_TIME m_FFBufferTime[MAX_BUFF_TIME];

    REFERENCE_TIME m_rtLastStart;   // rtStart for last delivered frame
    int m_nCountEstimated;          // Number of rtStart estimated since last rtStart received
    double m_dRate;
    REFERENCE_TIME m_rtPrevStop;
    bool m_bFrame_repeat_pict;

    bool m_bUseDXVA;
    CSize m_par;
    SwsContext* m_pSwsContext;
    unsigned __int64 m_nOutCsp;
    CSize m_pOutSize;               // Picture size on output pin
    int m_nSwOutBpp;

    // === DXVA common variables
    VIDEO_OUTPUT_FORMATS* m_pVideoOutputFormat;
    int m_nVideoOutputCount;
    DXVA_MODE m_nDXVAMode;
    CDXVADecoder* m_pDXVADecoder;
    GUID m_DXVADecoderGUID;

    DWORD m_nPCIVendor;
    DWORD m_nPCIDevice;
    LARGE_INTEGER m_VideoDriverVersion;
    CString m_strDeviceDescription;

    // === DXVA1 variables
    DDPIXELFORMAT m_PixelFormat;

    // === DXVA2 variables
    CComPtr<IDirect3DDeviceManager9> m_pDeviceManager;
    CComPtr<IDirectXVideoDecoderService> m_pDecoderService;
    CComPtr<IDirect3DSurface9> m_pDecoderRenderTarget;
    DXVA2_ConfigPictureDecode m_DXVA2Config;
    HANDLE m_hDevice;
    DXVA2_VideoDesc m_VideoDesc;

    CH264RandomAccess m_h264RandomAccess;

    BOOL m_bWaitingForKeyFrame;

    REFERENCE_TIME m_rtStart;

    // === Private functions
    void Cleanup();
    int  FindCodec(const CMediaType* mtIn);
    void AllocExtradata(AVCodecContext* pAVCtx, const CMediaType* mt);
    bool IsMultiThreadSupported(enum AVCodecID nCodec);
    void GetOutputFormats(int& nNumber, VIDEO_OUTPUT_FORMATS** ppFormats);
    void CalcAvgTimePerFrame();
    void DetectVideoCard(HWND hWnd);
    unsigned __int64 GetCspFromMediaType(GUID& subtype);
    void InitSwscale();

    void SetTypeSpecificFlags(IMediaSample* pMS);
    HRESULT SoftwareDecode(IMediaSample* pIn, BYTE* pDataIn, int nSize, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
    bool AppendBuffer(BYTE* pDataIn, int nSize, REFERENCE_TIME rtStart, REFERENCE_TIME rtStop);
    bool FindPicture(int nIndex, int nStartCode);
    void ShrinkBuffer();
    void ResetBuffer();
    void PushBufferTime(int nPos, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
    void PopBufferTime(int nPos);

public:

    const static AMOVIESETUP_MEDIATYPE sudPinTypesIn[];
    const static int sudPinTypesInCount;
    const static AMOVIESETUP_MEDIATYPE sudPinTypesOut[];
    const static int sudPinTypesOutCount;

    CMPCDXVAVideoDecFilter(LPUNKNOWN lpunk, HRESULT* phr);
    virtual ~CMPCDXVAVideoDecFilter();

    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
    virtual bool IsVideoInterlaced();
    virtual void GetOutputSize(int& w, int& h, int& arx, int& ary, int& RealWidth, int& RealHeight);
    CTransformOutputPin* GetOutputPin() { return m_pOutput; }

    void UpdateFrameTime(REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop, bool b_repeat_pict = false);
    CString GetFileExtension();

    void SetDXVAFilters(CAutoPtr<bool> DXVAFilters) { m_DXVAFilters = DXVAFilters; };

    // === Overriden DirectShow functions
    HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType* pmt);
    HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT Transform(IMediaSample* pIn);
    HRESULT CompleteConnect(PIN_DIRECTION direction, IPin* pReceivePin);
    HRESULT DecideBufferSize(IMemAllocator* pAllocator, ALLOCATOR_PROPERTIES* pProperties);
    HRESULT BeginFlush();
    HRESULT EndFlush();
    HRESULT NewSegment(REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, double dRate);
    HRESULT EndOfStream();

    HRESULT BreakConnect(PIN_DIRECTION dir);


    // === ISpecifyPropertyPages2

    STDMETHODIMP GetPages(CAUUID* pPages);
    STDMETHODIMP CreatePage(const GUID& guid, IPropertyPage** ppPage);

    // === IMPCDXVAVideoDecFilter
    STDMETHODIMP Apply();
    STDMETHOD_(GUID*, GetDXVADecoderGuid());
    STDMETHOD(SetActiveCodecs(MPC_VIDEO_CODEC nValue));
    STDMETHOD_(MPC_VIDEO_CODEC, GetActiveCodecs());
    STDMETHODIMP_(LPCTSTR) GetVideoCardDescription();

    STDMETHOD(SetARMode(int nValue));
    STDMETHOD_(int, GetARMode());
    STDMETHOD(SetDXVACheckCompatibility(int nValue));
    STDMETHOD_(int, GetDXVACheckCompatibility());

    STDMETHOD(SetDXVA_SD(int nValue));
    STDMETHOD_(int, GetDXVA_SD());

    // === IMPCDXVAVideoDecFilter2
    STDMETHOD_(int, GetFrameType());

    STDMETHOD(SetInterlacedFlag(MPCVD_INTERLACED_FLAG interlacedFlag));
    STDMETHOD_(MPCVD_INTERLACED_FLAG, GetInterlacedFlag());

    // === DXVA common functions
    BOOL IsSupportedDecoderConfig(const D3DFORMAT nD3DFormat, const DXVA2_ConfigPictureDecode& config, bool& bIsPrefered);
    BOOL IsSupportedDecoderMode(const GUID& mode);
    void BuildDXVAOutputFormat();
    int  GetPicEntryNumber();
    int  PictWidth();
    int  PictHeight();
    int  PictWidthRounded();
    int  PictHeightRounded();

    inline bool UseDXVA2() { return (m_nDXVAMode == MODE_DXVA2); };
    inline AVCodecContext* GetAVCtx() { return m_pAVCtx; };
    inline AVFrame* GetFrame() { return m_pFrame; };
    inline enum AVCodecID GetCodec() { return m_nCodecId; };
    inline bool IsReorderBFrame() { return m_bReorderBFrame; };
    inline bool IsEvo() { return m_bIsEVO; };
    inline DWORD GetPCIVendor() { return m_nPCIVendor; };
    inline REFERENCE_TIME GetAvrTimePerFrame() { return m_rtAvrTimePerFrame; };
    inline double GetRate() { return m_dRate; };
    bool IsDXVASupported();
    void UpdateAspectRatio();
    void ReorderBFrames(REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop);
    void FlushDXVADecoder()  {
        if (m_pDXVADecoder) {
            m_pDXVADecoder->Flush();
        }
    }

    void SetFrameType(FF_FIELD_TYPE nFrameType);

    // === DXVA1 functions
    DDPIXELFORMAT* GetPixelFormat() { return &m_PixelFormat; }
    HRESULT FindDXVA1DecoderConfiguration(IAMVideoAccelerator* pAMVideoAccelerator, const GUID* guidDecoder, DDPIXELFORMAT* pPixelFormat);
    HRESULT CheckDXVA1Decoder(const GUID* pGuid);
    void SetDXVA1Params(const GUID* pGuid, DDPIXELFORMAT* pPixelFormat);
    WORD GetDXVA1RestrictedMode();
    HRESULT CreateDXVA1Decoder(IAMVideoAccelerator* pAMVideoAccelerator, const GUID* pDecoderGuid, DWORD dwSurfaceCount);


    // === DXVA2 functions
    void FillInVideoDescription(DXVA2_VideoDesc* pDesc);
    DXVA2_ConfigPictureDecode* GetDXVA2Config() { return &m_DXVA2Config; };
    HRESULT ConfigureDXVA2(IPin* pPin);
    HRESULT SetEVRForDXVA2(IPin* pPin);
    HRESULT FindDXVA2DecoderConfiguration(IDirectXVideoDecoderService* pDecoderService,
                                          const GUID& guidDecoder,
                                          DXVA2_ConfigPictureDecode* pSelectedConfig,
                                          BOOL* pbFoundDXVA2Configuration);
    HRESULT CreateDXVA2Decoder(UINT nNumRenderTargets, IDirect3DSurface9** pDecoderRenderTargets);
};
