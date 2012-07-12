/*
 * Copyright (c) 2006-2007 h.yamagata
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "TffdshowDecVideoOutputPin.h"
#include "TffdshowVideoInputPin.h"
#include "TpresetSettingsVideo.h"
#include <dxva.h>

#include "codecs\TvideoCodecLibavcodec.h"
#include "codecs\TvideoCodecLibavcodecDxva.h"
#include "TffdshowDecVideoAllocatorDXVA.h"

TffdshowDecVideoOutputPin::TffdshowDecVideoOutputPin(
    TCHAR *pObjectName,
    TffdshowDecVideo *Ifdv,
    HRESULT * phr,
    LPCWSTR pName)
    : CTransformOutputPin(pObjectName, Ifdv, phr, pName),
      fdv(Ifdv),
      queue(NULL),
      oldSettingOfMultiThread(-1),
      isFirstFrame(true),
      pDXVA2Allocator(NULL),
      dwDXVA1SurfaceCount(0),
      guidDecoderDXVA1(GUID_NULL)
{
    DPRINTF(_l("TffdshowDecVideoOutputPin::Constructor"));
    memset(&ddUncompPixelFormat, 0, sizeof(ddUncompPixelFormat));
}

TffdshowDecVideoOutputPin::~TffdshowDecVideoOutputPin()
{
    DPRINTF(_l("TffdshowDecVideoOutputPin::Destructor"));
    freeQueue();
}

void TffdshowDecVideoOutputPin::freeQueue(void)
{
    if (queue) {
        queue->SetPopEvent(NULL);
        CloseHandle(hEvent);
        delete queue;
        queue =  NULL;
        hEvent = NULL;
    }
}

HRESULT TffdshowDecVideoOutputPin::Deliver(IMediaSample * pSample)
{
    if (m_pInputPin == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    if (!isFirstFrame && fdv->inpin->m_rateAndFlush.m_flushing) {
        return S_FALSE;
    }

    isFirstFrame = false;
    if (fdv->isQueue == 1) {
        ASSERT(queue);
        pSample->AddRef();
        return queue->Receive(pSample);
    } else {
        return m_pInputPin->Receive(pSample);
    }
}

void TffdshowDecVideoOutputPin::waitUntillQueueCleanedUp(void)
{
    if (queue == NULL) {
        return;
    }

    ResetEvent(hEvent);
    while (!queue->IsIdle()) {
        WaitForSingleObject(hEvent, INFINITE);
    }
}

void TffdshowDecVideoOutputPin::waitForPopEvent(void)
{
    if (queue == NULL) {
        return;
    }

    if (!queue->IsIdle()) {
        WaitForSingleObject(hEvent, INFINITE);
    }
}

void TffdshowDecVideoOutputPin::resetPopEvent(void)
{
    if (queue == NULL) {
        return;
    }

    ResetEvent(hEvent);
}

HRESULT TffdshowDecVideoOutputPin::DeliverBeginFlush(void)
{
    // DPRINTF(_l("TffdshowDecVideoOutputPin::DeliverBeginFlush"));
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    if (fdv->isQueue == 1) {
        queue->BeginFlush();
    } else {
        return m_Connected->BeginFlush();
    }
    return S_OK;
}

HRESULT TffdshowDecVideoOutputPin::DeliverEndFlush(void)
{
    // DPRINTF(_l("TffdshowDecVideoOutputPin::DeliverEndFlush"));
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    if (fdv->isQueue == 1) {
        queue->EndFlush();
    } else {
        return m_Connected->EndFlush();
    }
    return S_OK;
}

HRESULT TffdshowDecVideoOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
    DPRINTF(_l("TffdshowDecVideoOutputPin::DeliverNewSegment tStart %7.0f, tStop %7.0f"), tStart / 10000.0, tStop / 10000.0);
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }

    isFirstFrame = true;
    if (fdv->isQueue == 1) {
        DPRINTF(_l("queue->NewSegment"));
        queue->NewSegment(tStart, tStop, dRate);
    } else {
        DPRINTF(_l("m_Connected->NewSegment"));
        return m_Connected->NewSegment(tStart, tStop, dRate);
    }
    return S_OK;
}

HRESULT TffdshowDecVideoOutputPin::DeliverEndOfStream(void)
{
    DPRINTF(_l("TffdshowDecVideoOutputPin::DeliverEndOfStream"));
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }
    if (fdv->isQueue == 1) {
        queue->EOS();
        waitUntillQueueCleanedUp();
    } else {
        return m_Connected->EndOfStream();
    }
    return S_OK;
}

void TffdshowDecVideoOutputPin::SendAnyway(void)
{
    if (queue == NULL) {
        return;
    }

    queue->SendAnyway();
    waitUntillQueueCleanedUp();
}

HRESULT TffdshowDecVideoOutputPin::Inactive(void)
{
    DPRINTF(_l("TffdshowDecVideoOutputPin::Inactive"));
    if (m_Connected == NULL) {
        return VFW_E_NOT_CONNECTED;
    }

    if (fdv->isQueue == 1) {
        waitUntillQueueCleanedUp();
        queue->Reset();
    }
    return CBaseOutputPin::Inactive();
}

IMediaSample* TffdshowDecVideoOutputPin::GetBuffer(void)
{
    return NULL;
}

HRESULT TffdshowDecVideoOutputPin::CompleteConnect(IPin *pReceivePin)
{
    HRESULT phr = S_OK;
    DPRINTF(_l("TffdshowDecVideoOutputPin::CompleteConnect"));
    HRESULT hr = CTransformOutputPin::CompleteConnect(pReceivePin);
    if (SUCCEEDED(hr)) {
        if (queue) {
            freeQueue();
        }
        queue = new TffOutputQueue(this, pReceivePin, &phr, false, true, 1, false);
        hEvent = CreateEvent(NULL, false, false, NULL);
        queue->SetPopEvent(hEvent);
    }
    return hr;
}

HRESULT TffdshowDecVideoOutputPin::BreakConnect(void)
{
    // Omit checking connected or not to support dynamic reconnect.
    if (queue) {
        freeQueue();
    }
    m_pTransformFilter->BreakConnect(PINDIR_OUTPUT);
    return CBaseOutputPin::BreakConnect();
}

STDMETHODIMP TffdshowDecVideoOutputPin::Disconnect(void)
{
    // Omit checking connected or not to support dynamic reconnect.
    CAutoLock cObjectLock(m_pLock);
    return DisconnectInternal();
}

/* Asked to connect to a pin. A pin is always attached to an owning filter
   object so we always delegate our locking to that object. We first of all
   retrieve a media type enumerator for the input pin and see if we accept
   any of the formats that it would ideally like, failing that we retrieve
   our enumerator and see if it will accept any of our preferred types */

STDMETHODIMP TffdshowDecVideoOutputPin::Connect(
    IPin * pReceivePin,
    const AM_MEDIA_TYPE *pmt   // optional media type
)
{
    CheckPointer(pReceivePin, E_POINTER);
    ValidateReadPtr(pReceivePin, sizeof(IPin));
    CAutoLock cObjectLock(m_pLock);
    DisplayPinInfo(pReceivePin);

    // Omit checking connected or not to support dynamic reconnect.

    // Find a mutually agreeable media type -
    // Pass in the template media type. If this is partially specified,
    // each of the enumerated media types will need to be checked against
    // it. If it is non-null and fully specified, we will just try to connect
    // with this.

    DPRINTF(_l("TffdshowDecVideoOutputPin::Connect"));
    this->fdv->ConnectCompatibleFilter();
#if 0
    PIN_INFO pininfo;
    FILTER_INFO filterinfo;
    pReceivePin->QueryPinInfo(&pininfo);
    if (pininfo.pFilter) {
        pininfo.pFilter->QueryFilterInfo(&filterinfo);
        DPRINTF(_l("connenting to : filter=%s pin=%s"), filterinfo.achName, pininfo.achName);
        if (filterinfo.pGraph) {
            filterinfo.pGraph->Release();
        }
        pininfo.pFilter->Release();
    }
    BITMAPINFOHEADER *bmi = NULL;
    if (pmt && pmt->formattype == FORMAT_VideoInfo2
            && pmt->pbFormat
            && pmt->cbFormat >= sizeof(VIDEOINFOHEADER2)) {
        VIDEOINFOHEADER2* vih = (VIDEOINFOHEADER2*)pmt->pbFormat;
        bmi = &vih->bmiHeader;
    }
    if (pmt && pmt->formattype == FORMAT_VideoInfo
            && pmt->pbFormat
            && pmt->cbFormat >= sizeof(VIDEOINFOHEADER)) {
        VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)pmt->pbFormat;
        bmi = &vih->bmiHeader;
    }
#endif
    fdv->set_downstreamID(pReceivePin);
    const CMediaType * ptype = (CMediaType*)pmt;
    HRESULT hr = AgreeMediaType(pReceivePin, ptype);
    if (FAILED(hr)) {
        DPRINTF(_l("Failed to agree type"));
        this->fdv->DisconnectFromCompatibleFilter();
        // Since the procedure is already returning an error code, there
        // is nothing else this function can do to report the error.
        EXECUTE_ASSERT(SUCCEEDED(BreakConnect()));
        return hr;
    }

    DPRINTF(_l("Connection succeeded"));
    return NOERROR;
}

HRESULT TffdshowDecVideoOutputPin::GetDeliveryBuffer(IMediaSample ** ppSample,
        REFERENCE_TIME * pStartTime,
        REFERENCE_TIME * pEndTime,
        DWORD dwFlags)
{
    if (!m_pAllocator) {
        return E_NOINTERFACE;
    }
    return m_pAllocator->GetBuffer(ppSample, pStartTime, pEndTime, dwFlags);
}

STDMETHODIMP TffdshowDecVideoOutputPin::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
    if (riid == __uuidof(IAMVideoAcceleratorNotify)) {
        return GetInterface((IAMVideoAcceleratorNotify*)this, ppv);
    }
    return __super::NonDelegatingQueryInterface(riid, ppv);
}

//CBaseOutputPin
HRESULT TffdshowDecVideoOutputPin::InitAllocator(IMemAllocator **ppAlloc)
{
    DPRINTF(_l("TffdshowDecVideoOutputPin::InitAllocator"));
    TvideoCodecDec *pCodec = NULL;
    this->fdv->getMovieSource((const TvideoCodecDec **)&pCodec);

    if (pCodec->useDXVA() == 2) {
        DPRINTF(_l("TffdshowDecVideoOutputPin::InitAllocator DXVA mode"));
        //TODO : improve this by using interfaces (TvideoCodecLibavcodecDxva may not be the only decoder that would be used for DXVA purpose)
        TvideoCodecLibavcodecDxva *pDxvaCodec = (TvideoCodecLibavcodecDxva*)pCodec;

        HRESULT hr = S_FALSE;
        pDXVA2Allocator = new TffdshowDecVideoAllocatorDXVA(fdv, &hr);
        if (!pDXVA2Allocator) {
            return E_OUTOFMEMORY;
        }
        if (FAILED(hr)) {
            delete pDXVA2Allocator;
            return hr;
        }
        // Return the IMemAllocator interface.
        return pDXVA2Allocator->QueryInterface(__uuidof(IMemAllocator), (void **)ppAlloc);
    }
    return __super::InitAllocator(ppAlloc);
}


// === IAMVideoAcceleratorNotify
STDMETHODIMP TffdshowDecVideoOutputPin::GetUncompSurfacesInfo(const GUID *pGuid, LPAMVAUncompBufferInfo pUncompBufferInfo)
{
    DPRINTF(_l("TffdshowDecVideoOutputPin::GetUncompSurfacesInfo DXVA1 checking"));
    HRESULT hr = E_INVALIDARG;
    TvideoCodecLibavcodecDxva *pCodec = NULL;
    this->fdv->getMovieSource((const TvideoCodecDec **)&pCodec);

    if (SUCCEEDED(pCodec->checkDXVA1Decoder(pGuid))) {
        CComQIPtr<IAMVideoAccelerator> pAMVideoAccelerator = GetConnected();

        if (pAMVideoAccelerator) {
            pUncompBufferInfo->dwMaxNumSurfaces = pCodec->getPicEntryNumber();
            pUncompBufferInfo->dwMinNumSurfaces = pCodec->getPicEntryNumber();

            hr = pCodec->findDXVA1DecoderConfiguration(pAMVideoAccelerator, pGuid, &pUncompBufferInfo->ddUncompPixelFormat);
            if (SUCCEEDED(hr)) {
                memcpy(&ddUncompPixelFormat, &pUncompBufferInfo->ddUncompPixelFormat, sizeof(DDPIXELFORMAT));
                guidDecoderDXVA1 = *pGuid;
            }
        }
    }
    DPRINTF(_l("TffdshowDecVideoOutputPin::GetUncompSurfacesInfo DXVA1 checking result (%x)"), hr);
    return hr;
}

STDMETHODIMP TffdshowDecVideoOutputPin::SetUncompSurfacesInfo(DWORD dwActualUncompSurfacesAllocated)
{
    dwDXVA1SurfaceCount = dwActualUncompSurfacesAllocated;
    return S_OK;
}

STDMETHODIMP TffdshowDecVideoOutputPin::GetCreateVideoAcceleratorData(const GUID *pGuid, LPDWORD pdwSizeMiscData, LPVOID *ppMiscData)
{
    HRESULT                        hr                    = E_UNEXPECTED;
    AMVAUncompDataInfo             UncompInfo;
    AMVACompBufferInfo             CompInfo[COMP_BUFFER_COUNT];
    DWORD                          dwNumTypesCompBuffers = countof(CompInfo);
    CComQIPtr<IAMVideoAccelerator> pAMVideoAccelerator   = GetConnected();
    DXVA_ConnectMode*              pConnectMode;

    DPRINTF(_l("TffdshowDecVideoOutputPin::GetCreateVideoAcceleratorData DXVA1 create decoder"));

    if (pAMVideoAccelerator) {
        TvideoCodecLibavcodecDxva *pCodec = NULL;
        this->fdv->getMovieSource((const TvideoCodecDec **)&pCodec);

        memcpy(&UncompInfo.ddUncompPixelFormat, &ddUncompPixelFormat, sizeof(DDPIXELFORMAT));
        UncompInfo.dwUncompWidth        = pCodec->pictWidthRounded();
        UncompInfo.dwUncompHeight       = pCodec->pictHeightRounded();
        hr = pAMVideoAccelerator->GetCompBufferInfo(&guidDecoderDXVA1, &UncompInfo, &dwNumTypesCompBuffers, CompInfo);

        if (SUCCEEDED(hr)) {

            hr = pCodec->createDXVA1Decoder(pAMVideoAccelerator, pGuid, dwDXVA1SurfaceCount);

            if (SUCCEEDED(hr)) {
                pCodec->setDXVA1Params(&guidDecoderDXVA1, &ddUncompPixelFormat);

                pConnectMode                    = (DXVA_ConnectMode*)CoTaskMemAlloc(sizeof(DXVA_ConnectMode));
                pConnectMode->guidMode          = guidDecoderDXVA1;
                pConnectMode->wRestrictedMode   = pCodec->getDXVA1RestrictedMode();
                *pdwSizeMiscData                = sizeof(DXVA_ConnectMode);
                *ppMiscData                     = pConnectMode;
            }
        }
    }
    DPRINTF(_l("TffdshowDecVideoOutputPin::GetCreateVideoAcceleratorData DXVA1 create decoder result (%x)"), hr);
    return hr;
}
