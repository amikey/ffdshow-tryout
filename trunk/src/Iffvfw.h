#ifndef _IFFVFW_H_
#define _IFFVFW_H_

// {E8F1B2B2-17C8-4f20-990B-27BDFF03C407}
DEFINE_GUID(IID_Iffvfw , 0xe8f1b2b2, 0x17c8, 0x4f20, 0x99, 0x0b, 0x27, 0xbd, 0xff, 0x03, 0xc4, 0x07);
// {05F983EC-637F-4133-B489-5E03914929D7}
DEFINE_GUID(CLSID_FFVFW, 0x05f983ec, 0x637f, 0x4133, 0xb4, 0x89, 0x5e, 0x03, 0x91, 0x49, 0x29, 0xd7);

#define FFDSHOWVFW_DESC_L L"ffdshow Video Codec"

DECLARE_INTERFACE_(Iffvfw, IUnknown)
{
    STDMETHOD_(void, aboutDlg)(HWND hParent) = 0;
    STDMETHOD_(void, configDlg)(HWND hParent) = 0;
    STDMETHOD_(int, saveMem)(void * buf, int len) = 0;
    STDMETHOD_(int, loadMem)(const void * buf, int len) = 0;
    STDMETHOD_(void, loadReg)(void) = 0;

    STDMETHOD_(LRESULT, coQuery)(BITMAPINFO*, BITMAPINFO*) = 0;
    STDMETHOD_(LRESULT, coGetFormat)(BITMAPINFO*, BITMAPINFO*) = 0;
    STDMETHOD_(LRESULT, coGetSize)(BITMAPINFO*, BITMAPINFO*) = 0;
    STDMETHOD_(LRESULT, coFramesInfo)(ICCOMPRESSFRAMES*) = 0;
    STDMETHOD_(LRESULT, coBegin)(BITMAPINFO*, BITMAPINFO*) = 0;
    STDMETHOD_(LRESULT, coEnd)(void) = 0;
    STDMETHOD_(LRESULT, coRun)(void*) = 0;

    STDMETHOD_(LRESULT, decQuery)(BITMAPINFO*, BITMAPINFO*) = 0;
    STDMETHOD_(LRESULT, decGetFormat)(BITMAPINFO*, BITMAPINFO*) = 0;
    STDMETHOD_(LRESULT, decBegin)(BITMAPINFO*, BITMAPINFO*) = 0;
    STDMETHOD_(LRESULT, decEnd)(void) = 0;
    STDMETHOD_(LRESULT, decRun)(void*) = 0;
};

#endif
