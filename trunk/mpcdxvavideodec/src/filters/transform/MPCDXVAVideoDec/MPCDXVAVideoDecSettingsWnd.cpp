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
#include "resource.h"
#include "MPCDXVAVideoDecSettingsWnd.h"
#include "../../../DSUtil/DSUtil.h"

#include "ffmpeg_dxva/libavcodec/avcodec.h"

//
// CMPCDXVAVideoDecSettingsWnd
//

int g_AVDiscard[] = {
    AVDISCARD_NONE,
    AVDISCARD_DEFAULT,
    AVDISCARD_NONREF,
    AVDISCARD_BIDIR,
    AVDISCARD_NONKEY,
    AVDISCARD_ALL,
};

int FindDiscardIndex(int nValue)
{
    for (int i = 0; i < _countof(g_AVDiscard); i++) {
        if (g_AVDiscard[i] == nValue) {
            return i;
        }
    }
    return 1;
}

int g_AVErrRecognition[] = {
    AV_EF_CAREFUL,
    AV_EF_COMPLIANT,
    AV_EF_AGGRESSIVE,
};

int FindErrRecognitionIndex(int nValue)
{
    for (int i = 0; i < _countof(g_AVErrRecognition); i++) {
        if (g_AVErrRecognition[i] == nValue) {
            return i;
        }
    }
    return 1;
}

CMPCDXVAVideoDecSettingsWnd::CMPCDXVAVideoDecSettingsWnd()
{
}

bool CMPCDXVAVideoDecSettingsWnd::OnConnect(const CInterfaceList<IUnknown, &IID_IUnknown>& pUnks)
{
    ASSERT(!m_pMDF);

    m_pMDF.Release();

    POSITION pos = pUnks.GetHeadPosition();
    while (pos && !(m_pMDF = pUnks.GetNext(pos))) {
        ;
    }

    if (!m_pMDF) {
        return false;
    }

    return true;
}

void CMPCDXVAVideoDecSettingsWnd::OnDisconnect()
{
    m_pMDF.Release();
}

bool CMPCDXVAVideoDecSettingsWnd::OnActivate()
{
    ASSERT(IPP_FONTSIZE == 13);
    const int h20 = IPP_SCALE(20);
    const int h25 = IPP_SCALE(25);
    DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP;
    CPoint p(10, 10);
    GUID* DxvaGui = nullptr;

    //m_grpFFMpeg.Create(ResStr(IDS_VDF_FFSETTINGS), WS_VISIBLE | WS_CHILD | BS_GROUPBOX, CRect(p + CPoint(-5, 0), CSize(IPP_SCALE(350), h20 + h25 * 3 + h20)), this, (UINT)IDC_STATIC);
    //p.y += h20;

    // Read AR from stream
    m_cbARMode.Create(ResStr(IDS_VDF_AR_MODE), dwStyle | BS_AUTOCHECKBOX | BS_LEFTTEXT, CRect(p, CSize(IPP_SCALE(340), m_fontheight)), this, IDC_PP_AR);
    m_cbARMode.SetCheck(FALSE);
    p.y += h25;

    // Interlaced flag
    m_txtInterlacedFlag.Create(ResStr(IDS_VDF_INTERLACED_FLAG), WS_VISIBLE | WS_CHILD, CRect(p, CSize(IPP_SCALE(220), m_fontheight)), this, (UINT)IDC_STATIC);
    m_cbInterlacedFlag.Create(dwStyle | CBS_DROPDOWNLIST | WS_VSCROLL, CRect(p + CPoint(IPP_SCALE(230), -6), CSize(IPP_SCALE(110), 200)), this, IDC_PP_INTERLACED_FLAG);
    m_cbInterlacedFlag.AddString(ResStr(IDS_VDF_AUTO));
    m_cbInterlacedFlag.AddString(ResStr(IDS_VDF_IF_PROGRESSIVE));
    m_cbInterlacedFlag.AddString(ResStr(IDS_VDF_IF_TOP_FIELD_FIRST));
    m_cbInterlacedFlag.AddString(ResStr(IDS_VDF_IF_BOTTOM_FIELD_FIRST));
    p.y += h25;

    //m_grpDXVA.Create(ResStr(IDS_VDF_DXVA_SETTING), WS_VISIBLE | WS_CHILD | BS_GROUPBOX, CRect(p + CPoint(-5, 0), CSize(IPP_SCALE(350), h20 + h25 + h20 * 3 + m_fontheight)), this, (UINT)IDC_STATIC);
    //p.y += h20;

    // DXVA Compatibility check
    m_txtDXVACompatibilityCheck.Create(ResStr(IDS_VDF_DXVACOMPATIBILITY), WS_VISIBLE | WS_CHILD, CRect(p, CSize(IPP_SCALE(225), m_fontheight)), this, (UINT)IDC_STATIC);
    m_cbDXVACompatibilityCheck.Create(dwStyle | CBS_DROPDOWNLIST | WS_VSCROLL, CRect(p + CPoint(IPP_SCALE(230), -4), CSize(IPP_SCALE(110), 200)), this, IDC_PP_DXVA_CHECK);
    m_cbDXVACompatibilityCheck.AddString(ResStr(IDS_VDF_DXVA_FULLCHECK));
    m_cbDXVACompatibilityCheck.AddString(ResStr(IDS_VDF_DXVA_SKIP_LEVELCHECK));
    m_cbDXVACompatibilityCheck.AddString(ResStr(IDS_VDF_DXVA_SKIP_REFCHECK));
    m_cbDXVACompatibilityCheck.AddString(ResStr(IDS_VDF_DXVA_SKIP_ALLCHECK));
    p.y += h25;

    // Set DXVA for SD (H.264)
    m_cbDXVA_SD.Create(ResStr(IDS_VDF_DXVA_SD), dwStyle | BS_AUTOCHECKBOX | BS_LEFTTEXT, CRect(p, CSize(IPP_SCALE(340), m_fontheight)), this, IDC_PP_DXVA_SD);
    m_cbDXVA_SD.SetCheck(FALSE);
    p.y += h25;

    // DXVA mode
    m_txtDXVAMode.Create(ResStr(IDS_VDF_DXVA_MODE), WS_VISIBLE | WS_CHILD, CRect(p, CSize(IPP_SCALE(120), m_fontheight)), this, (UINT)IDC_STATIC);
    m_edtDXVAMode.Create(WS_CHILD | WS_VISIBLE | WS_DISABLED, CRect(p + CPoint(IPP_SCALE(120), 0), CSize(IPP_SCALE(220), m_fontheight)), this, 0);
    p.y += h20;

    // Video card description
    m_txtVideoCardDescription.Create(ResStr(IDS_VDF_VIDEOCARD), WS_VISIBLE | WS_CHILD, CRect(p, CSize(IPP_SCALE(120), m_fontheight)), this, (UINT)IDC_STATIC);
    m_edtVideoCardDescription.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_DISABLED, CRect(p + CPoint(IPP_SCALE(120), 0), CSize(IPP_SCALE(220), m_fontheight * 2)), this, 0);
    m_edtVideoCardDescription.SetWindowText(m_pMDF->GetVideoCardDescription());

    DxvaGui = m_pMDF->GetDXVADecoderGuid();
    if (DxvaGui != nullptr) {
        CString DXVAMode = GetDXVAMode(DxvaGui);
        m_edtDXVAMode.SetWindowText(DXVAMode);
    } else {
        m_txtDXVAMode.ShowWindow(SW_HIDE);
        m_edtDXVAMode.ShowWindow(SW_HIDE);
    }

    for (CWnd* pWnd = GetWindow(GW_CHILD); pWnd; pWnd = pWnd->GetNextWindow()) {
        pWnd->SetFont(&m_font, FALSE);
    }

    CorrectComboListWidth(m_cbDXVACompatibilityCheck);

    if (m_pMDF) { 
        m_cbARMode.SetCheck(m_pMDF->GetARMode());
    	 
        m_cbDXVACompatibilityCheck.SetCurSel(m_pMDF->GetDXVACheckCompatibility());
        m_cbDXVA_SD.SetCheck(m_pMDF->GetDXVA_SD());

        m_cbInterlacedFlag.SetCurSel(m_pMDF->GetInterlacedFlag());
    }

    return true;
}

void CMPCDXVAVideoDecSettingsWnd::OnDeactivate()
{
}

bool CMPCDXVAVideoDecSettingsWnd::OnApply()
{
    OnDeactivate();

    if (m_pMDF && m_cbDXVACompatibilityCheck.m_hWnd) {
        m_pMDF->SetARMode(m_cbARMode.GetCheck());

        m_pMDF->SetDXVACheckCompatibility(m_cbDXVACompatibilityCheck.GetCurSel());

        m_pMDF->SetDXVA_SD(m_cbDXVA_SD.GetCheck());

        m_pMDF->SetInterlacedFlag((MPCVD_INTERLACED_FLAG)m_cbInterlacedFlag.GetCurSel());

        m_pMDF->Apply();
    }

    return true;
}


BEGIN_MESSAGE_MAP(CMPCDXVAVideoDecSettingsWnd, CInternalPropertyPageWnd)
END_MESSAGE_MAP()


// ====== Codec filter property page (for standalone filter only)

CMPCDXVAVideoDecCodecWnd::CMPCDXVAVideoDecCodecWnd()
{
}

bool CMPCDXVAVideoDecCodecWnd::OnConnect(const CInterfaceList<IUnknown, &IID_IUnknown>& pUnks)
{
    ASSERT(!m_pMDF);

    m_pMDF.Release();

    POSITION pos = pUnks.GetHeadPosition();
    while (pos && !(m_pMDF = pUnks.GetNext(pos))) {
        ;
    }

    if (!m_pMDF) {
        return false;
    }

    return true;
}

void CMPCDXVAVideoDecCodecWnd::OnDisconnect()
{
    m_pMDF.Release();
}

bool CMPCDXVAVideoDecCodecWnd::OnActivate()
{
    DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER;
    int nPos = 0;
    MPC_VIDEO_CODEC nActiveCodecs = (MPC_VIDEO_CODEC)(m_pMDF ? m_pMDF->GetActiveCodecs() : 0);

    m_grpSelectedCodec.Create(_T("Selected codecs"), WS_VISIBLE | WS_CHILD | BS_GROUPBOX, CRect(10,  10, 330, 280), this, (UINT)IDC_STATIC);

    m_lstCodecs.Create(dwStyle | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP, CRect(20, 30, 320, 270), this, 0);

    m_lstCodecs.AddString(_T("H.264/AVC"));
    m_lstCodecs.SetCheck(nPos++, (nActiveCodecs & MPCVD_H264_DXVA) != 0);

    m_lstCodecs.AddString(_T("VC1"));
    m_lstCodecs.SetCheck(nPos++, (nActiveCodecs & MPCVD_VC1_DXVA) != 0);

    for (CWnd* pWnd = GetWindow(GW_CHILD); pWnd; pWnd = pWnd->GetNextWindow()) {
        pWnd->SetFont(&m_font, FALSE);
    }

    return true;
}

void CMPCDXVAVideoDecCodecWnd::OnDeactivate()
{
}

bool CMPCDXVAVideoDecCodecWnd::OnApply()
{
    OnDeactivate();

    if (m_pMDF) {
        int nActiveCodecs = 0;
        int nPos = 0;

        if (m_lstCodecs.GetCheck(nPos++)) {
            nActiveCodecs |= MPCVD_H264_DXVA;
        }
        if (m_lstCodecs.GetCheck(nPos++)) {
            nActiveCodecs |= MPCVD_VC1_DXVA;
        }
        m_pMDF->SetActiveCodecs((MPC_VIDEO_CODEC)nActiveCodecs);

        m_pMDF->Apply();
    }

    return true;
}


BEGIN_MESSAGE_MAP(CMPCDXVAVideoDecCodecWnd, CInternalPropertyPageWnd)
END_MESSAGE_MAP()
