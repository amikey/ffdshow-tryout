/*
 * Copyright (c) 2004-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Coutsfs.h"
#include "TsampleFormat.h"
#include "ToutputAudioSettings.h"
#include "dsutil.h"
#include "Tlibavcodec.h"
#include "Ttranslate.h"

const int ToutsfsPage::ac3bitrates[] = {
    32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640
};

const char_t* ToutsfsPage::bitstream_formats[] = {
    _l("Try all formats (default)"),
    _l("Try only standard formats"),
    _l("Only Xonar formats"),
    NULL
};

void ToutsfsPage::init(void)
{
    for (int i = 0; i < countof(ac3bitrates); i++) {
        char_t s[30];
        cbxAdd(IDC_CBX_OUT_AC3, _itoa(ac3bitrates[i], s, 10), ac3bitrates[i]);
    }
    Tlibavcodec *lavc;
    deci->getLibavcodec(&lavc);
    static const int ac3s[] = {IDC_CHB_OUT_AC3, IDC_LBL_OUT_AC3, IDC_CBX_OUT_AC3, IDC_CHB_AOUT_AC3ENCODE_MODE, 0};
    enable(lavc && lavc->ok, ac3s);
    if (lavc) {
        lavc->Release();
    }

    setCheck(IDC_CHB_PASSTHROUGH_PCM_CONNECT  , cfgGet(IDFF_aoutpassthroughPCMConnection) == 1);

    addHint(IDC_CHB_OUT_AC3, _l("Will encode the output stream to AC3 format. AC3 encoder currently supports the following sample rates: 32kHz and 48kHz.\nother sample rates should be resampled to one of the supported sample rates,\nthis can be done using the \"Resample\" filter."));
    addHint(IDC_CHB_ALWAYEXTENSIBLE, _l("\"not needed\": no custom channel mapping"));
    addHint(IDC_CHB_ALLOWOUTSTREAM, _l("Useful for directly storing encoded ac3 to a file in graphedt with File Writer filter"));
    addHint(IDC_CHB_PASSTHROUGH_EAC3, _l("Performs a Dolby Digital Plus (EAC3) bitstream if your HT receiver supports it."));
    addHint(IDC_CHB_PASSTHROUGH_TRUEHD, _l("Performs a Dolby TrueHD bitstream if your HT receiver supports it."));
    addHint(IDC_CHB_PASSTHROUGH_DTSHD, _l("Performs a DTS HD bitstream if your HT receiver supports it."));
    addHint(IDC_CBX_OUT_PASSTHROUGH_DEVICEID, _l("Change this parameter only if your card does not support the official media types"));
    addHint(IDC_CHB_PASSTHROUGH_PCM_CONNECT, _l("Some renderers need that the connection is done on PCM (uncompressed) format first then it would switch to bitstream. If checked then FFDShow will assume that your receiver supports the bitstream formats you checked"));
    warningShowed = 0;
}

void ToutsfsPage::cfg2dlg(void)
{
    setCheck(IDC_CHB_PASSTHROUGH_AC3  , cfgGet(IDFF_aoutpassthroughAC3) == 1);
    setCheck(IDC_CHB_PASSTHROUGH_DTS  , cfgGet(IDFF_aoutpassthroughDTS) == 1);
    setCheck(IDC_CHB_PASSTHROUGH_TRUEHD, cfgGet(IDFF_aoutpassthroughTRUEHD) == 1);
    setCheck(IDC_CHB_PASSTHROUGH_DTSHD, cfgGet(IDFF_aoutpassthroughDTSHD) == 1);
    setCheck(IDC_CHB_PASSTHROUGH_EAC3, cfgGet(IDFF_aoutpassthroughEAC3) == 1);
    int outsfs = cfgGet(IDFF_outsfs);
    ac32dlg(outsfs);
    setCheck(IDC_CHB_OUT_PCM16  , outsfs & TsampleFormat::SF_PCM16);
    setCheck(IDC_CHB_OUT_PCM24  , outsfs & TsampleFormat::SF_PCM24);
    setCheck(IDC_CHB_OUT_PCM32  , outsfs & TsampleFormat::SF_PCM32);
    setCheck(IDC_CHB_OUT_FLOAT32, outsfs & TsampleFormat::SF_FLOAT32);
    setCheck(IDC_CHB_OUT_LPCM   , outsfs & TsampleFormat::SF_LPCM16);

    setCheck(IDC_CHB_ALWAYEXTENSIBLE, !cfgGet(IDFF_alwaysextensible));
    setCheck(IDC_CHB_ALLOWOUTSTREAM, cfgGet(IDFF_allowOutStream));
    setCheck(IDC_CHB_PASSTHROUGH_PCM_CONNECT, cfgGet(IDFF_aoutpassthroughPCMConnection) == 1);
    cbxSetCurSel(IDC_CBX_OUT_PASSTHROUGH_DEVICEID, cfgGet(IDFF_aoutpassthroughDeviceId));
    SetDlgItemInt(m_hwnd, IDC_ED_AUDIO_DELAY, cfgGet(IDFF_audio_decoder_delay), TRUE);

    connect2dlg();
}

void ToutsfsPage::getTip(char_t *tipS, size_t len)
{
    if (cfgGet(IDFF_videoDelay)) {
        strncatf(tipS, len, _l("\nAudio delay: %i msec"), cfgGet(IDFF_audio_decoder_delay));
    }
}

void ToutsfsPage::ac32dlg(int &outsfs)
{
    if (enabled(IDC_CHB_OUT_AC3)) {
        int isAC3 = outsfs & TsampleFormat::SF_AC3;
        setCheck(IDC_CHB_OUT_AC3, isAC3);
        cbxSetDataCurSel(IDC_CBX_OUT_AC3, cfgGet(IDFF_outAC3bitrate));
        static const int ac3s[] = {IDC_LBL_OUT_AC3, IDC_CBX_OUT_AC3, IDC_CHB_AOUT_AC3ENCODE_MODE, 0};
        enable(isAC3, ac3s, FALSE);
        setCheck(IDC_CHB_AOUT_AC3ENCODE_MODE, (cfgGet(IDFF_aoutAC3EncodeMode) == 1));
    }
}

void ToutsfsPage::connect2dlg(void)
{
    int connect = cfgGet(IDFF_aoutConnectTo);
    cbxSetCurSel(IDC_CBX_AOUT_CONNECTTO, connect);
    enable(connect > 0, IDC_CHB_AOUT_CONNECTTO_SPDIF);
    setCheck(IDC_CHB_AOUT_CONNECTTO_SPDIF, cfgGet(IDFF_aoutConnectToOnlySpdif));
}

INT_PTR ToutsfsPage::msgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CHB_PASSTHROUGH_AC3:
                case IDC_CHB_PASSTHROUGH_DTS:
                case IDC_CHB_PASSTHROUGH_TRUEHD:
                case IDC_CHB_PASSTHROUGH_DTSHD:
                case IDC_CHB_PASSTHROUGH_EAC3:
                case IDC_CHB_PASSTHROUGH_PCM_CONNECT: {
                    cfgSet(IDFF_aoutpassthroughAC3, getCheck(IDC_CHB_PASSTHROUGH_AC3));
                    cfgSet(IDFF_aoutpassthroughDTS, getCheck(IDC_CHB_PASSTHROUGH_DTS));
                    cfgSet(IDFF_aoutpassthroughTRUEHD, getCheck(IDC_CHB_PASSTHROUGH_TRUEHD));
                    cfgSet(IDFF_aoutpassthroughDTSHD, getCheck(IDC_CHB_PASSTHROUGH_DTSHD));
                    cfgSet(IDFF_aoutpassthroughEAC3, getCheck(IDC_CHB_PASSTHROUGH_EAC3));
                    cfgSet(IDFF_aoutpassthroughPCMConnection, getCheck(IDC_CHB_PASSTHROUGH_PCM_CONNECT));
                    break;
                }
                case IDC_CHB_OUT_LPCM:
                    setCheck(IDC_CHB_OUT_PCM16, 0);
                    setCheck(IDC_CHB_OUT_PCM24, 0);
                    setCheck(IDC_CHB_OUT_PCM32, 0);
                    setCheck(IDC_CHB_OUT_FLOAT32, 0);
                    setCheck(IDC_CHB_OUT_AC3, 0);
                case IDC_CHB_OUT_AC3:
                case IDC_CHB_OUT_PCM16:
                case IDC_CHB_OUT_PCM24:
                case IDC_CHB_OUT_PCM32:
                case IDC_CHB_OUT_FLOAT32: {
                    int outsfs = 0;
                    if (getCheck(IDC_CHB_OUT_PCM16)) {
                        outsfs |= TsampleFormat::SF_PCM16;
                    }
                    if (getCheck(IDC_CHB_OUT_PCM24)) {
                        outsfs |= TsampleFormat::SF_PCM24;
                    }
                    if (getCheck(IDC_CHB_OUT_PCM32)) {
                        outsfs |= TsampleFormat::SF_PCM32;
                    }
                    if (getCheck(IDC_CHB_OUT_FLOAT32)) {
                        outsfs |= TsampleFormat::SF_FLOAT32;
                    }
                    if (enabled(IDC_CHB_OUT_AC3) && getCheck(IDC_CHB_OUT_AC3)) {
                        outsfs |= TsampleFormat::SF_AC3;
                    }

                    if (getCheck(IDC_CHB_OUT_LPCM))
                        if (outsfs == 0) {
                            outsfs = TsampleFormat::SF_LPCM16;
                        } else {
                            setCheck(IDC_CHB_OUT_LPCM, 0);
                        }
                    if (outsfs) {
                        cfgSet(IDFF_outsfs, outsfs);
                    } else {
                        setCheck(LOWORD(wParam), !getCheck(LOWORD(wParam)));
                    }
                    if (enabled(IDC_CHB_OUT_AC3)) {
                        static const int ac3s[] = {IDC_LBL_OUT_AC3, IDC_CBX_OUT_AC3, IDC_CHB_AOUT_AC3ENCODE_MODE, 0};
                        enable(getCheck(IDC_CHB_OUT_AC3), ac3s, FALSE);
                    }
                    return TRUE;
                }
            }
            break;
    }
    return TconfPageDecAudio::msgProc(uMsg, wParam, lParam);
}

void ToutsfsPage::applySettings(void)
{
    if (cfgGet(IDFF_aoutConnectTo) == 2) {
        DWORD merit;
        if (FAILED(getFilterMerit(HKEY_CLASSES_ROOT, _l("{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default WaveOut Device"), &merit)))
            if (FAILED(getFilterMerit(HKEY_CURRENT_USER, _l("Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default WaveOut Device"), &merit))) {
                return;
            }
        if (merit <= MERIT_DO_NOT_USE && MessageBox(m_hwnd, _(-IDD_OUTSFS, _l("WaveOut Device merit is too low. Do you want to raise it?")), _(-IDD_OUTSFS, _l("Low merit")), MB_ICONQUESTION | MB_YESNO) == IDYES) {
            setFilterMerit(HKEY_CLASSES_ROOT, _l("{E0F158E1-CB04-11d0-BD4E-00A0C911CE86}\\InstanceCm\\Default WaveOut Device"), MERIT_NORMAL);
            setFilterMerit(HKEY_CURRENT_USER, _l("Software\\Microsoft\\ActiveMovie\\devenum\\{E0F158E1-CB04-11D0-BD4E-00A0C911CE86}\\Default WaveOut Device"), MERIT_NORMAL);
        }
    }
}

bool ToutsfsPage::reset(bool testonly)
{
    if (!testonly) {
        deci->resetParam(IDFF_alwaysextensible);
        deci->resetParam(IDFF_allowOutStream);
        deci->resetParam(IDFF_aoutAC3EncodeMode);
    }
    return true;
}

void ToutsfsPage::translate(void)
{
    TconfPageDec::translate();

    cbxTranslate(IDC_CBX_AOUT_CONNECTTO, ToutputAudioSettings::connetTos);
    cbxTranslate(IDC_CBX_OUT_PASSTHROUGH_DEVICEID, ToutsfsPage::bitstream_formats);
}

void ToutsfsPage::dlg2compatmode(void)
{
    if (!warningShowed) {
        MessageBox(hwndParent, _l("Changing this parameter should only be done if your card (like asus xonar) support HD audio bitstream but doesn't work with FFDShow (auzentech xfi not supported)"), _l("Caution"), 0);
    }
    warningShowed = 1;
    // If in compatibility mode publish a PCM format to enable connection (the format will be dynamically changed after the connection)
    if (cbxGetCurItemData(IDC_CBX_OUT_PASSTHROUGH_DEVICEID) == 0) {
        setCheck(IDC_CHB_PASSTHROUGH_PCM_CONNECT, 0);
    } else {
        setCheck(IDC_CHB_PASSTHROUGH_PCM_CONNECT, 1);
    }
}

ToutsfsPage::ToutsfsPage(TffdshowPageDec *Iparent): TconfPageDecAudio(Iparent)
{
    dialogId = IDD_OUTSFS;
    inPreset = 1;
    idffOrder = maxOrder + 3;
    filterID = IDFF_filterOutputAudio;
    static const TbindCheckbox<ToutsfsPage> chb[] = {
        IDC_CHB_ALWAYEXTENSIBLE, -IDFF_alwaysextensible, NULL,
        IDC_CHB_ALLOWOUTSTREAM, IDFF_allowOutStream, NULL,
        IDC_CHB_AOUT_CONNECTTO_SPDIF, IDFF_aoutConnectToOnlySpdif, NULL,
        IDC_CHB_AOUT_AC3ENCODE_MODE, IDFF_aoutAC3EncodeMode, NULL,
        IDC_CHB_ALWAYEXTENSIBLE_IEC61937, IDFF_aoutUseIEC61937, NULL,
        0, NULL, NULL
    };
    bindCheckboxes(chb);
    static const TbindCombobox<ToutsfsPage> cbx[] = {
        IDC_CBX_OUT_AC3, IDFF_outAC3bitrate, BINDCBX_DATA, NULL,
        IDC_CBX_AOUT_CONNECTTO, IDFF_aoutConnectTo, BINDCBX_SEL, &ToutsfsPage::connect2dlg,
        IDC_CBX_OUT_PASSTHROUGH_DEVICEID, IDFF_aoutpassthroughDeviceId, BINDCBX_SEL, NULL,
        0
    };
    bindComboboxes(cbx);

    static const TbindEditInt<ToutsfsPage> edInt[] = {
        IDC_ED_AUDIO_DELAY, INT_MIN / 2, INT_MAX / 2, IDFF_audio_decoder_delay, NULL,
        0, NULL, NULL
    };
    bindEditInts(edInt);
}
