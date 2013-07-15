/*
 * (C) 2006-2013 see Authors.txt
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

// Internal codec list (used to enable/disable codec in standalone mode)
typedef enum {
    MPCVD_H264_DXVA  = 1 << 11,
    MPCVD_VC1_DXVA   = 1 << 12
} MPC_VIDEO_CODEC;

// Interlaced flag handling
enum MPCVD_INTERLACED_FLAG {
    MPCVC_INTERLACED_AUTO,
    MPCVC_INTERLACED_PROGRESSIVE,
    MPCVC_INTERLACED_TOP_FIELD_FIRST,
    MPCVC_INTERLACED_BOTTOM_FIELD_FIRST
};

interface __declspec(uuid("CDC3B5B3-A8B0-4c70-A805-9FC80CDEF262"))
IMPCDXVAVideoDecFilter :
public IUnknown {
    STDMETHOD(Apply()) = 0;

    STDMETHOD_(GUID*, GetDXVADecoderGuid()) = 0;

    STDMETHOD(SetActiveCodecs(MPC_VIDEO_CODEC nValue)) = 0;
    STDMETHOD_(MPC_VIDEO_CODEC, GetActiveCodecs()) = 0;

    STDMETHOD_(LPCTSTR, GetVideoCardDescription()) = 0;

    STDMETHOD(SetARMode(int nValue)) = 0;
    STDMETHOD_(int, GetARMode()) = 0;
    STDMETHOD(SetDXVACheckCompatibility(int nValue)) = 0;
    STDMETHOD_(int, GetDXVACheckCompatibility()) = 0;

    STDMETHOD(SetDXVA_SD(int nValue)) = 0;
    STDMETHOD_(int, GetDXVA_SD()) = 0;
};

interface __declspec(uuid("F0ABC515-19ED-4D65-9D5F-59E36AE7F2AF"))
IMPCDXVAVideoDecFilter2 :
public IMPCDXVAVideoDecFilter {
    STDMETHOD_(int, GetFrameType()) = 0;

    STDMETHOD(SetInterlacedFlag(MPCVD_INTERLACED_FLAG interlacedFlag)) = 0;
    STDMETHOD_(MPCVD_INTERLACED_FLAG, GetInterlacedFlag()) = 0;
};
