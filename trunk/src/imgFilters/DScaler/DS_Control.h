/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 John Adcock.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// This header file is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details
/////////////////////////////////////////////////////////////////////////////
//
// Notes for writers of external apps
//
// To use control DScaler using an external app you can use SendMessage to
// perform both commands and to get/set all the settings
//
// To perform commands use the WM_COMMAND message
// e.g. SendMessage(hWndDScaler, WM_COMMAND, IDM_OSD_SHOW, 0);
//
// To get settings use the appropriate WM_XXX_GETVALUE
// e.g. Brightness = SendMessage(hWndDScaler, WM_BT848_GETVALUE, BRIGHTNESS, 0);
//
// To set settings use the appropriate WM_XXX_SETVALUE
// e.g. SendMessage(hWndDScaler, WM_BT848_SETVALUE, HUE, NewHueValue);
//
// To up settings use the appropriate WM_XXX_CHANGEVALUE
// e.g. SendMessage(hWndDScaler, WM_BT848_CHANGEVALUE, HUE, INCREMENTVALUE);
//
// To down settings use the appropriate WM_XXX_CHANGEVALUE
// e.g. SendMessage(hWndDScaler, WM_BT848_CHANGEVALUE, HUE, DECREMENTVALUE);
//
// To show settings use the appropriate WM_XXX_CHANGEVALUE
// e.g. SendMessage(hWndDScaler, WM_BT848_CHANGEVALUE, HUE, DISPLAYVALUE);
//
// To reset settings to default use the appropriate WM_XXX_CHANGEVALUE
// e.g. SendMessage(hWndDScaler, WM_BT848_CHANGEVALUE, HUE, RESETVALUE);
//
// To do the above operation without using the OSD use the ????VALUE_SILENT
// e.g. SendMessage(hWndDScaler, WM_BT848_CHANGEVALUE, HUE, INCREMENTVALUE_SILENT);
//
// The DScaler window handle can be obtained using
// hWndDScaler = FindWindow(DSCALER_APPNAME, NULL);
//
/////////////////////////////////////////////////////////////////////////////
//
// Notes for DScaler developers
//
// This is the place to add settings for any new file you create
// You should also update the LoadSettingsFromIni & SaveSettingsToIni
// functions in Settings.h so that your setttings get loaded
//
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 16 Jan 2001   John Adcock           Moved all parts that could be used to
//                                     Control DScaler externally to this file
//
// 20 Feb 2001   Michael Samblanet     Added new values for Aspect Control
//                                     (bounce & clipping Modes)
//
// 08 Jun 2001   Eric Schmidt          Added bounce amplitude to ini
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __DS_CONTROL_H___
#define __DS_CONTROL_H___

#define DSCALER_APPNAME "DScaler"

/////////////////////////////////////////////////////////////////////////////
// Control Messages passed using WM_COMMAND
/////////////////////////////////////////////////////////////////////////////
#ifdef DSCALER_EXTERNAL

#define IDM_SAVE_SETTINGS_NOW           100

#define IDM_VT_RESET                    261
#define IDM_RESET                       280
#define IDM_TAKESTILL                   485
#define IDM_OVERLAY_STOP                590
#define IDM_OVERLAY_START               591
#define IDM_HIDE_OSD                    592
#define IDM_SHOW_OSD                    593
// IDM_SET_OSD_TEXT the lParam must be the handle of a global atom
#define IDM_SET_OSD_TEXT                594
#define IDM_FAST_REPAINT                599
#define IDM_ASPECT_FULLSCREEN           701
#define IDM_ASPECT_LETTERBOX            702
#define IDM_ASPECT_ANAMORPHIC           703

// Messages for the Video Format Menu
#define IDM_TYPEFORMAT_PAL              1120
#define IDM_TYPEFORMAT_NTSC             1121
#define IDM_TYPEFORMAT_SECAM            1122
#define IDM_TYPEFORMAT_PAL_M            1123
#define IDM_TYPEFORMAT_PAL_N            1124
#define IDM_TYPEFORMAT_NTSC_JAPAN       1125
#define IDM_TYPEFORMAT_PAL60            1126

// Indexes for the Video Deinterlace Algorithms
// to select a deinterlace method send a WM_COMMAND message
// with the relevant index added to this message
// e.g. To switch to adaptive
// SendMessage(hWndDScaler, WM_COMMAND, IDM_FIRST_DEINTMETHOD + INDEX_ADAPTIVE, 0);
#define IDM_FIRST_DEINTMETHOD             1900

/////////////////////////////////////////////////////////////////////////////
// For setting of certain values
//

enum eVideoFormat
{
    VIDEOFORMAT_PAL_B = 0,
    VIDEOFORMAT_PAL_D,
    VIDEOFORMAT_PAL_G,
    VIDEOFORMAT_PAL_H,
    VIDEOFORMAT_PAL_I,
    VIDEOFORMAT_PAL_M,
    VIDEOFORMAT_PAL_N,
    VIDEOFORMAT_PAL_60,
    VIDEOFORMAT_PAL_N_COMBO,

    VIDEOFORMAT_SECAM_B,
    VIDEOFORMAT_SECAM_D,
    VIDEOFORMAT_SECAM_G,
    VIDEOFORMAT_SECAM_H,
    VIDEOFORMAT_SECAM_K,
    VIDEOFORMAT_SECAM_K1,
    VIDEOFORMAT_SECAM_L,
    VIDEOFORMAT_SECAM_L1,

    VIDEOFORMAT_NTSC_M,
    VIDEOFORMAT_NTSC_M_Japan,
    VIDEOFORMAT_NTSC_50,

    VIDEOFORMAT_LASTONE
};

#endif

// Indexes for the Video Deinterlace Algorithms
#define INDEX_VIDEO_BOB                   0
#define INDEX_VIDEO_WEAVE                 1
#define INDEX_VIDEO_2FRAME                2
#define INDEX_WEAVE                       3
#define INDEX_BOB                         4
#define INDEX_SCALER_BOB                  5
#define INDEX_EVEN_ONLY                   13
#define INDEX_ODD_ONLY                    14
#define INDEX_BLENDED_CLIP                15
#define INDEX_ADAPTIVE                    16
#define INDEX_VIDEO_GREEDY                17
#define INDEX_VIDEO_GREEDY2FRAME          18
#define INDEX_VIDEO_GREEDYH               19
#define INDEX_OLD_GAME                    20
#define INDEX_VIDEO_TOMSMOCOMP            21
#define INDEX_VIDEO_MOCOMP2               22

/////////////////////////////////////////////////////////////////////////////
// Allow callers to convert a WM code to do other operations
/////////////////////////////////////////////////////////////////////////////

#define WM_CONVERT_TO_GETVALUE(x)     (((x - WM_APP) % 100) + WM_APP)
#define WM_CONVERT_TO_SETVALUE(x)    ((((x - WM_APP) % 100) + 100) + WM_APP)
#define WM_CONVERT_TO_CHANGEVALUE(x) ((((x - WM_APP) % 100) + 200) + WM_APP)


/////////////////////////////////////////////////////////////////////////////
// Constants for WM_????_CHANGEVALUE messages
/////////////////////////////////////////////////////////////////////////////
typedef enum
{
    DISPLAY = 0,          // Display OSD Value.
    ADJUSTUP,             // Increase Value, with acceleration [display OSD]
    ADJUSTDOWN,           // Decrease Value, with acceleration [display OSD]
    INCREMENT,            // Increase Value by 1 [display OSD]
    DECREMENT,            // Decrease Value by 1 [display OSD]
    RESET,                // Reset Value to default [display OSD]
    TOGGLEBOOL,           // Toggle a boolean setting [display OSD]
    ADJUSTUP_SILENT,      // Same, but no OSD
    ADJUSTDOWN_SILENT,    // Same, but no OSD
    INCREMENT_SILENT,     // Same, but no OSD
    DECREMENT_SILENT,     // Same, but no OSD
    RESET_SILENT,         // Same, but no OSD
    TOGGLEBOOL_SILENT,    // Same, but no OSD
} eCHANGEVALUE;

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in AspectRatio.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    SOURCE_ASPECT = 0,
    CUSTOM_SOURCE_ASPECT,
    TARGET_ASPECT,
    CUSTOM_TARGET_ASPECT,
    ASPECT_MODE,
    LUMINANCETHRESHOLD,
    IGNORENONBLACKPIXELS,
    AUTODETECTASPECT,
    ZOOMINFRAMECOUNT,
    ASPECTHISTORYTIME,
    ASPECTCONSISTENCYTIME,
    VERTICALPOS,
    HORIZONTALPOS,
    CLIPPING,
    BOUNCE,
    BOUNCEPERIOD,
    DEFERSETOVERLAY,
    BOUNCETIMERPERIOD,
    BOUNCEAMPLITUDE,
    ORBIT,
    ORBITPERIODX,
    ORBITPERIODY,
    ORBITSIZE,
    ORBITTIMERPERIOD,
    AUTOSIZEWINDOW,
    SKIPPERCENT,
    XZOOMFACTOR,
    YZOOMFACTOR,
    XZOOMCENTER,
    YZOOMCENTER,
    CHROMARANGE,
    WAITFORVERTBLANKINDRAW,
    ZOOMOUTFRAMECOUNT,
    ALLOWGREATERTHANSCREEN,
    MASKGREYSHADE,
    USEWSS,
    DEFAULTSOURCEASPECT,
	DEFAULTASPECTMODE,
    ASPECT_SETTING_LASTONE,
} ASPECT_SETTING;

#define WM_ASPECT_GETVALUE          (WM_APP + 1)
#define WM_ASPECT_SETVALUE          (WM_APP + 101)
#define WM_ASPECT_CHANGEVALUE       (WM_APP + 201)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in Bt848.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    BRIGHTNESS = 0,
    CONTRAST,
    HUE,
    SATURATION,
    SATURATIONU,
    SATURATIONV,
    TOPOVERSCAN,
    BDELAY,
    BTAGCDISABLE,
    BTCRUSH,
    BTEVENCHROMAAGC,
    BTODDCHROMAAGC,
    BTEVENLUMAPEAK,
    BTODDLUMAPEAK,
    BTFULLLUMARANGE,
    BTEVENLUMADEC,
    BTODDLUMADEC,
    BTEVENCOMB,
    BTODDCOMB,
    BTCOLORBARS,
    BTGAMMACORRECTION,
    BTCORING,
    BTHORFILTER,
    BTVERTFILTER,
    BTCOLORKILL,
    BTWHITECRUSHUP,
    BTWHITECRUSHDOWN,
    CURRENTX,
    CUSTOMPIXELWIDTH,
    VIDEOSOURCE,
    TVFORMAT,
    HDELAY,
    VDELAY,
    REVERSEPOLARITY,
    CURRENTCARDTYPE,
    CURRENTTUNERTYPE,
    AUDIOSOURCE1,
    AUDIOCHANNEL,
    AUTOSTEREOSELECT,
    VOLUME,
    BASS,
    TREBLE,
    BALANCE,
    BT848SAVEPERINPUT_DISABLED,
    BT848SAVEPERFORMAT_DISABLED,
    AUDIOSOURCE2,
    AUDIOSOURCE3,
    AUDIOSOURCE4,
    AUDIOSOURCE5,
    AUDIOSOURCE6,
    USEINPUTPIN1,
    USEEQUALIZER,
    EQUALIZERBAND1,
    EQUALIZERBAND2,
    EQUALIZERBAND3,
    EQUALIZERBAND4,
    EQUALIZERBAND5,
    LOUDNESS,
    SUPERBASS,
    SPATIAL,
    AUTOVOLUMECORRECTION,
    AUDIOSTANDARDDETECT,
    AUDIOSTANDARDDETECTINTERVAL,
    AUDIOSTANDARDMANUAL,
    AUDIOSTANDARDMAJORCARRIER,
    AUDIOSTANDARDMINORCARRIER,
    AUDIOSTANDARDINSTATUSBAR,
    MSP34XXFLAGS,
    AUTOSTEREODETECTINTERVAL,
    BOTTOMOVERSCAN,
    LEFTOVERSCAN,
    RIGHTOVERSCAN,
    BT848_SETTING_LASTONE,
} BT848_SETTING;

#define WM_BT848_GETVALUE           (WM_APP + 2)
#define WM_BT848_SETVALUE           (WM_APP + 102)
#define WM_BT848_CHANGEVALUE        (WM_APP + 202)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DScaler.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    STARTLEFT = 0,
    STARTTOP,
    STARTWIDTH,
    STARTHEIGHT,
    ALWAYSONTOP,
    ISFULLSCREEN,
    FORCEFULLSCREEN,
    SHOWSTATUSBAR,
    SHOWMENU,
    WINDOWPROCESSOR,
    THREADPROCESSOR,
    WINDOWPRIORITY,
    THREADPRIORITY,
    AUTOSAVESETTINGS,
    ALWAYSONTOPFULL,
    SHOWCRASHDIALOG,
    DISPLAYSPLASHSCREEN,
    AUTOHIDECURSOR,
    LOCKKEYBOARD,
	LOCKKEYBOARDMAINWINDOWONLY,
    SCREENSAVEROFF,
    AUTOCODEPAGE,
    VTANTIALIAS,
    INITIALSOURCE,
    CHANNELENTERTIME,
    PROCESSORSPEED,
    TRADEOFF,
    FULLCPU,
    VIDEOCARD,
	REVERSECHANNELSCROLLING,
    SINGLEKEYTELETEXTTOGGLE,
	MINTOTRAY,
	MINIMIZEHANDLING,
	RESOFULLSCREEN,
	PSTRIPRESO576I,
	PSTRIPRESO480I,
	SKINNAME,
	CHANNELPREVIEWWNBCOLS,
	CHANNELPREVIEWNBROWS,
    DSCALER_SETTING_LASTONE,
} DSCALER_SETTING;

#define WM_DSCALER_GETVALUE             (WM_APP + 3)
#define WM_DSCALER_SETVALUE             (WM_APP + 103)
#define WM_DSCALER_CHANGEVALUE          (WM_APP + 203)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in OutThreads.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    HURRYWHENLATE = 0,
    WAITFORFLIP,
    DOVERTICALFLIP,
    TUNERSWITCHSCREENUPDATEDELAY,
    DOACCURATEFLIPS,
    AUTODETECT,
    WAITFORVSYNC,
    DOJUDDERTERMINATORONVIDEO,
    OUTTHREADS_SETTING_LASTONE,
} OUTTHREADS_SETTING;

#define WM_OUTTHREADS_GETVALUE      (WM_APP + 4)
#define WM_OUTTHREADS_SETVALUE      (WM_APP + 104)
#define WM_OUTTHREADS_CHANGEVALUE   (WM_APP + 204)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in Other.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    BACKBUFFERS = 0,
    OVERLAYCOLOR,
    USEOVERLAYCONTROLS,
    OVERLAYBRIGHTNESS,
    OVERLAYCONTRAST,
    OVERLAYHUE,
    OVERLAYSATURATION,
    OVERLAYGAMMA,
    OVERLAYSHARPNESS,
    OTHER_SETTING_LASTONE,
} OTHER_SETTING;

#define WM_OTHER_GETVALUE           (WM_APP + 5)
#define WM_OTHER_SETVALUE           (WM_APP + 105)
#define WM_OTHER_CHANGEVALUE        (WM_APP + 205)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FD_50Hz.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    PULLDOWNTHRESHHOLDLOW = 0,
    PULLDOWNTHRESHHOLDHIGH,
    PALFILMFALLBACKMODE,
    PALFILMREPEATCOUNT,
    PALFILMREPEATCOUNT2,
    MAXCALLSTOPALCOMB,
    PALBADCADENCEMODE,
    FD50_SETTING_LASTONE,
} FD50_SETTING;

#define WM_FD50_GETVALUE            (WM_APP + 6)
#define WM_FD50_SETVALUE            (WM_APP + 106)
#define WM_FD50_CHANGEVALUE         (WM_APP + 206)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FD_50Hz.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    NTSCFILMFALLBACKMODE = 0,
    NTSCPULLDOWNREPEATCOUNT,
    NTSCPULLDOWNREPEATCOUNT2,
    THRESHOLD32PULLDOWN,
    THRESHOLDPULLDOWNMISMATCH,
    THRESHOLDPULLDOWNCOMB,
    FALLBACKTOVIDEO,
    PULLDOWNSWITCHINTERVAL,
    PULLDOWNSWITCHMAX,
    MAXCALLSTOCOMB,
    NTSCBADCADENCEMODE,
    FD60_SETTING_LASTONE,
} FD60_SETTING;

#define WM_FD60_GETVALUE            (WM_APP + 7)
#define WM_FD60_SETVALUE            (WM_APP + 107)
#define WM_FD60_CHANGEVALUE         (WM_APP + 207)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FD_Common.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    BITSHIFT = 0,
    EDGEDETECT,
    JAGGIETHRESHOLD,
    DIFFTHRESHOLD,
    USECHROMA,
    FILMFLIPDELAY,
    FD_COMMON_SETTING_LASTONE,
} FD_COMMON_SETTING;

#define WM_FD_COMMON_GETVALUE       (WM_APP + 8)
#define WM_FD_COMMON_SETVALUE       (WM_APP + 108)
#define WM_FD_COMMON_CHANGEVALUE    (WM_APP + 208)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_Adaptive.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    LOWMOTIONFIELDCOUNT = 0,
    STATICIMAGEFIELDCOUNT,
    STATICIMAGEMODE,
    LOWMOTIONMODE,
    HIGHMOTIONMODE,
    ADAPTIVETHRESH32PULLDOWN,
    ADAPTIVETHRESHMISMATCH,
    DI_ADAPTIVE_SETTING_LASTONE,
} DI_ADAPTIVE_SETTING;

#define WM_DI_ADAPTIVE_GETVALUE     (WM_APP + 9)
#define WM_DI_ADAPTIVE_SETVALUE     (WM_APP + 109)
#define WM_DI_ADAPTIVE_CHANGEVALUE  (WM_APP + 209)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_VideoBob.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    WEAVEEDGEDETECT = 0,
    WEAVEJAGGIETHRESHOLD,
    DI_VIDEOBOB_SETTING_LASTONE,
} DI_VIDEOBOB_SETTING;

#define WM_DI_VIDEOBOB_GETVALUE     (WM_APP + 10)
#define WM_DI_VIDEOBOB_SETVALUE     (WM_APP + 110)
#define WM_DI_VIDEOBOB_CHANGEVALUE  (WM_APP + 210)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_BlendedClip.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    BLCMINIMUMCLIP = 0,
    BLCPIXELMOTIONSENSE,
    BLCRECENTMOTIONSENSE,
    BLCMOTIONAVGPERIOD,
    BLCPIXELCOMBSENSE,
    BLCRECENTCOMBSENSE,
    BLCCOMBAVGPERIOD,
    BLCHIGHCOMBSKIP,
    BLCLOWMOTIONSKIP,
    BLCVERTICALSMOOTHING,
    BLCUSEINTERPBOB,
    BLCBLENDCHROMA,
    BLCSHOWCONTROLS,
    DI_BLENDEDCLIP_SETTING_LASTONE,
} DI_BLENDEDCLIP_SETTING;

#define WM_DI_BLENDEDCLIP_GETVALUE      (WM_APP + 11)
#define WM_DI_BLENDEDCLIP_SETVALUE      (WM_APP + 111)
#define WM_DI_BLENDEDCLIP_CHANGEVALUE   (WM_APP + 211)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_TwoFrame.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    TWOFRAMESPATIALTOLERANCE = 0,
    TWOFRAMETEMPORALTOLERANCE,
    DI_TWOFRAME_SETTING_LASTONE,
} DI_TWOFRAME_SETTING;

#define WM_DI_TWOFRAME_GETVALUE     (WM_APP + 12)
#define WM_DI_TWOFRAME_SETVALUE     (WM_APP + 112)
#define WM_DI_TWOFRAME_CHANGEVALUE  (WM_APP + 212)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_Greedy.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    GREEDYMAXCOMB = 0,
    GREEDYUSETILTNWARP,
    GREEDYSIZERATIO,
    GREEDYLINEARSIZEPCNT,
    GREEDYLINEARLOCIN,
    GREEDYLINEARLOCOUT,
    GREEDYTOPWARPFACTOR,
    GREEDYBOTTOMWARPFACTOR,
    DI_GREEDY_SETTING_LASTONE,
} DI_GREEDY_SETTING;

#define WM_DI_GREEDY_GETVALUE       (WM_APP + 15)
#define WM_DI_GREEDY_SETVALUE       (WM_APP + 115)
#define WM_DI_GREEDY_CHANGEVALUE    (WM_APP + 215)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_TNoise.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    TEMPORALLUMINANCETHRESHOLD = 0,
    TEMPORALCHROMATHRESHOLD,
    TEMPORALNOISEPREFETCH,
    LOCKTHRESHOLDSTOGETHER,
    USETEMPORALNOISEFILTER,
    FLT_TNOISE_SETTING_LASTONE,
} FLT_TNOISE_SETTING;

#define WM_FLT_TNOISE_GETVALUE      (WM_APP + 14)
#define WM_FLT_TNOISE_SETVALUE      (WM_APP + 114)
#define WM_FLT_TNOISE_CHANGEVALUE   (WM_APP + 214)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in OSD.c
/////////////////////////////////////////////////////////////////////////////
typedef enum
{
    OSDB_USERDEFINED    = -1,
    OSDB_TRANSPARENT    = 0,
    OSDB_BLOCK,
    OSDB_SHADED,
    OSDB_LASTONE,
} eOSDBackground;

typedef enum
{
    OSD_OUTLINECOLOR = 0,
    OSD_TEXTCOLOR,
    OSD_PERCENTAGESIZE,
    OSD_PERCENTAGESMALLSIZE,
    OSD_ANTIALIAS,
    OSD_BACKGROUND,
    OSD_OUTLINE,
    OSD_AUTOHIDE_SCREEN,
    OSD_USE_GENERAL_SCREEN,
    OSD_USE_STATISTICS_SCREEN,
    OSD_USE_WSS_SCREEN,
    OSD_USE_DEVELOPER_SCREEN,
	OSD_FONT,
    OSD_SETTING_LASTONE,
} OSD_SETTING;
#define WM_OSD_GETVALUE     (WM_APP + 18)
#define WM_OSD_SETVALUE     (WM_APP + 118)
#define WM_OSD_CHANGEVALUE  (WM_APP + 218)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_Gamma.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    GAMMAVALUE = 0,
    USESTOREDTABLE,
    USEGAMMAFILTER,
    BLACKLEVEL,
    WHITELEVEL,
    FLT_GAMMA_SETTING_LASTONE,
} FLT_GAMMA_SETTING;

#define WM_FLT_GAMMA_GETVALUE       (WM_APP + 20)
#define WM_FLT_GAMMA_SETVALUE       (WM_APP + 120)
#define WM_FLT_GAMMA_CHANGEVALUE    (WM_APP + 220)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in VBI.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    CAPTURE_VBI = 0,
    CLOSEDCAPTIONMODE,
    DOTELETEXT,
    DOVPS,
    DOWSS,
    SEARCHHIGHLIGHT,
    VBI_SETTING_LASTONE,
} VBI_SETTING;

#define WM_VBI_GETVALUE     (WM_APP + 21)
#define WM_VBI_SETVALUE     (WM_APP + 121)
#define WM_VBI_CHANGEVALUE  (WM_APP + 221)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_Greedy2Frame.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    GREEDYTWOFRAMETHRESHOLDLUMA = 0,
    GREEDYTWOFRAMETHRESHOLDCHROMA,
    DI_GREEDY2FRAME_SETTING_LASTONE,
} DI_GREEDY2FRAME_SETTING;

#define WM_DI_GREEDY2FRAME_GETVALUE     (WM_APP + 22)
#define WM_DI_GREEDY2FRAME_SETVALUE     (WM_APP + 122)
#define WM_DI_GREEDY2FRAME_CHANGEVALUE  (WM_APP + 222)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_VideoWeave.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    TEMPORALTOLERANCE = 0,
    SPATIALTOLERANCE,
    SIMILARITYTHRESHOLD,
    DI_VIDEOWEAVE_SETTING_LASTONE,
} DI_VIDEOWEAVE_SETTING;

#define WM_DI_VIDEOWEAVE_GETVALUE       (WM_APP + 23)
#define WM_DI_VIDEOWEAVE_SETVALUE       (WM_APP + 123)
#define WM_DI_VIDEOWEAVE_CHANGEVALUE    (WM_APP + 223)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_LinearCorrection.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USELINEARCORRFILTER = 0,
    DOONLYMASKING,
    MASKTYPE,
    MASKPARAM1,
    MASKPARAM2,
    MASKPARAM3,
    MASKPARAM4,
    FLT_LINEAR_CORR_SETTING_LASTONE,
} FLT_LINEAR_CORR_SETTING;

#define WM_FLT_LINEAR_CORR_GETVALUE     (WM_APP + 24)
#define WM_FLT_LINEAR_CORR_SETVALUE     (WM_APP + 124)
#define WM_FLT_LINEAR_CORR_CHANGEVALUE  (WM_APP + 224)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in MixerDev.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USEMIXER = 0,
    DESTINDEX,
    INPUT1INDEX,
    INPUT2INDEX,
    INPUT3INDEX,
    INPUT4INDEX,
    MIXERRESETONEXIT,
    INPUT5INDEX,
    INPUT6INDEX,
	MIXERNAME,
    NOHARDWAREMUTE,
    MIXERDEV_SETTING_LASTONE,
} MIXERDEV_SETTING;

#define WM_MIXERDEV_GETVALUE        (WM_APP + 25)
#define WM_MIXERDEV_SETVALUE        (WM_APP + 125)
#define WM_MIXERDEV_CHANGEVALUE     (WM_APP + 225)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FieldTiming.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    AUTOFORMATDETECT = 0,
    FIFTY_HZ_FORMAT,
    SIXTY_HZ_FORMAT,
    FORMATCHANGETHRESHOLD,
    SLEEPINTERVAL,
    SLEEPSKIPFIELDS,
    SLEEPSKIPFIELDSLATE,
	MAXFIELDSHIFT,
	ALWAYSSLEEP,
    GIVEUPPROCDURINGJT,
    TIMING_SETTING_LASTONE,
} TIMING_SETTING;

#define WM_TIMING_GETVALUE          (WM_APP + 26)
#define WM_TIMING_SETVALUE          (WM_APP + 126)
#define WM_TIMING_CHANGEVALUE       (WM_APP + 226)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in Program.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    COUNTRYCODE = 0,
    CURRENTPROGRAM,
    SCANMODE,
    USEAFCWHILESCANNING,
    CHANNELS_SETTING_LASTONE,
} CHANNELS_SETTING;

#define WM_CHANNELS_GETVALUE            (WM_APP + 27)
#define WM_CHANNELS_SETVALUE            (WM_APP + 127)
#define WM_CHANNELS_CHANGEVALUE         (WM_APP + 227)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in Audio.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
	SYSTEMINMUTE = 0,
    AUDIO_SETTING_LASTONE,
} AUDIO_SETTING;

#define WM_AUDIO_GETVALUE               (WM_APP + 28)
#define WM_AUDIO_SETVALUE               (WM_APP + 128)
#define WM_AUDIO_CHANGEVALUE            (WM_APP + 228)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in Debug.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    DEBUGLOG = 0,
    DEBUGLEVEL,
    DEBUGFLUSHAFTERWRITE,
	DEBUGLOGFILE,
    DEBUG_SETTING_LASTONE,
} DEBUG_SETTING;

#define WM_DEBUG_GETVALUE               (WM_APP + 29)
#define WM_DEBUG_SETVALUE               (WM_APP + 129)
#define WM_DEBUG_CHANGEVALUE            (WM_APP + 229)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_LogoKill.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    LOGOKILL_TOP = 0,
    LOGOKILL_LEFT,
    LOGOKILL_WIDTH,
    LOGOKILL_HEIGHT,
    LOGOKILL_MODE,
    LOGOKILL_MAX,
	LOGOKILL_USE_SMOOTHING,
	LOGOKILL_SMOOTHING_AMOUNT,
    USELOGOKILL,
    FLT_LOGOKILL_SETTING_LASTONE,
} FLT_LOGOKILL_SETTING;

#define WM_FLT_LOGOKILL_GETVALUE        (WM_APP + 30)
#define WM_FLT_LOGOKILL_SETVALUE        (WM_APP + 130)
#define WM_FLT_LOGOKILL_CHANGEVALUE     (WM_APP + 230)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in VT.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    VT_CODEPAGE = 0,
    VT_LANGUAGE_REGION,
    VT_SHOW_SUBCODE_OSD,
    VT_LINES_CACHINGCONTROL,
    VT_HIGH_GRANULARITY_CACHING,
    VT_SUBSTITUTE_ERROR_SPACES,
    VT_SUBTITLE_DUPLICATION_FILTER,
    VT_DOUBLEHEIGHT_SUBTITLES_FILTER,
    VT_SETTING_LASTONE,
} VT_SETTING;

#define WM_VT_GETVALUE     (WM_APP + 31)
#define WM_VT_SETVALUE     (WM_APP + 131)
#define WM_VT_CHANGEVALUE  (WM_APP + 231)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_GreedyH.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
	GR_MAX_COMB = 0,
	GR_MOTION_THRESHOLD,
	GR_MOTION_SENSE,
	GR_GOOD_PULLDOWN_LVL,
	GR_BAD_PULLDOWN_LVL,
	GR_H_SHARPNESS,
	GR_V_SHARPNESS,
	GR_MEDIAN_FILTER,
	GR_LOW_MOTION_PD_LVL,
	GR_USE_PULLDOWN,
	GR_USE_IN_BETWEEN,
	GR_USE_MEDIAN_FILTER,
	GR_USE_V_SHARPNESS,
	GR_USE_H_SHARPNESS,
//	GR_USE_LOW_MOTION_ONLY,
	DI_GREEDYH_SETTING_LASTONE,
} DI_GREEDYH_SETTING;

#define WM_DI_GREEDYH_GETVALUE		(WM_APP + 32)
#define WM_DI_GREEDYH_SETVALUE		(WM_APP + 132)
#define WM_DI_GREEDYH_CHANGEVALUE	(WM_APP + 232)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_OldGame.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    DI_OLDGAME_MAXCOMB = 0,
    DI_OLDGAME_COMPOSITEMODE,
    DI_OLDGAME_SETTING_LASTONE,
} DI_OLDGAME_SETTING;

#define WM_DI_OLDGAME_GETVALUE     (WM_APP + 33)
#define WM_DI_OLDGAME_SETVALUE     (WM_APP + 133)
#define WM_DI_OLDGAME_CHANGEVALUE  (WM_APP + 233)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_Sharpness.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    SHARPNESS = 0,
    SHARPNESS_PREFETCH,
    USESHARPNESS,
    FLT_SHARPNESS_SETTING_LASTONE,
} FLT_SHARPNESS_SETTING;

#define WM_FLT_SHARPNESS_GETVALUE     (WM_APP + 34)
#define WM_FLT_SHARPNESS_SETVALUE     (WM_APP + 134)
#define WM_FLT_SHARPNESS_CHANGEVALUE  (WM_APP + 234)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_TemporalComb.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    TCOMB_MAX_PHASE_COLOR_DIFF = 0,
    TCOMB_PREFETCH,
    TCOMB_SHIMMER_HIST_PERCENT,
    TCOMB_SHIMMER_PERCENT,
    TCOMB_ACTIVATE,
    TCOMB_TRADE_SPEED_FOR_ACCURACY,
	TCOMB_VIDOE_MODE,
    FLT_TCOMB_SETTING_LASTONE,
} FLT_TCOMB_SETTING;

#define WM_FLT_TCOMB_GETVALUE     (WM_APP + 35)
#define WM_FLT_TCOMB_SETVALUE     (WM_APP + 135)
#define WM_FLT_TCOMB_CHANGEVALUE  (WM_APP + 235)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in Calibration.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    LEFT_SOURCE_CROPPING = 0,
    RIGHT_SOURCE_CROPPING,
    SHOW_RGB_DELTA,
    SHOW_YUV_DELTA,
    CALIBR_SETTING_LASTONE,
} CALIBR_SETTING;

#define WM_CALIBR_GETVALUE     (WM_APP + 36)
#define WM_CALIBR_SETVALUE     (WM_APP + 136)
#define WM_CALIBR_CHANGEVALUE  (WM_APP + 236)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DVBT.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    DVBT_SETTING_LASTONE = 0,
} DVBT_SETTING;

#define WM_DVBT_GETVALUE     (WM_APP + 37)
#define WM_DVBT_SETVALUE     (WM_APP + 137)
#define WM_DVBT_CHANGEVALUE  (WM_APP + 237)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_GradualNoise.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    NOISE_REDUCTION = 0,
    GNOISE_PREFETCH,
    GNOISE_ACTIVATE,
    FLT_GNOISE_SETTING_LASTONE,
} FLT_GNOISE_SETTING;

#define WM_FLT_GNOISE_GETVALUE      (WM_APP + 38)
#define WM_FLT_GNOISE_SETVALUE      (WM_APP + 138)
#define WM_FLT_GNOISE_CHANGEVALUE   (WM_APP + 238)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_AdaptiveNoise.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    ANOISE_STABILITY = 0,
    ANOISE_PREFETCH,
    ANOISE_REDUCTION,
    ANOISE_LOCK_DOT,
    ANOISE_READOUT,
    ANOISE_ACTIVATE,
    ANOISE_INDICATOR,
    ANOISE_MOTION_MEMORY,
    ANOISE_DO_SPATIAL,
    ANOISE_SPATIAL,
    FLT_ANOISE_SETTING_LASTONE,
} FLT_ANOISE_SETTING;

#define WM_FLT_ANOISE_GETVALUE      (WM_APP + 39)
#define WM_FLT_ANOISE_SETVALUE      (WM_APP + 139)
#define WM_FLT_ANOISE_CHANGEVALUE   (WM_APP + 239)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_Histogram.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    HISTOGRAM_DISPLAY_MODE = 0,
    HISTOGRAM_PREFETCH,
    HISTOGRAM_ACTIVATE,
    HISTOGRAM_COMB_MODE,
    FLT_HISTOGRAM_SETTING_LASTONE,
} FLT_HISTOGRAM_SETTING;

#define WM_FLT_HISTOGRAM_GETVALUE      (WM_APP + 40)
#define WM_FLT_HISTOGRAM_SETVALUE      (WM_APP + 140)
#define WM_FLT_HISTOGRAM_CHANGEVALUE   (WM_APP + 240)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in StillSource.cpp
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    FORMATSAVING = 0,
    SLIDESHOWDELAY,
    JPEGQUALITY,
    PATTERNHEIGTH,
    PATTERNWIDTH,
    DELAYBETWEENSTILLS,
    SAVEINSAMEFILE,
    STILLSINMEMORY,
    NBCONSECUTIVESTILLS,
	KEEPORIGINALRATIO,
	OSDFORSTILLS,
	STILLPREVIEWWNBCOLS,
	STILLPREVIEWNBROWS,
	MAXMEMFORSTILLS,
	SAVINGPATH,
    STILL_SETTING_LASTONE,
} STILL_SETTING;

#define WM_STILL_GETVALUE     (WM_APP + 41)
#define WM_STILL_SETVALUE     (WM_APP + 141)
#define WM_STILL_CHANGEVALUE  (WM_APP + 241)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_Mirror.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USEMIRROR,
    FLT_MIRROR_SETTING_LASTONE,
} FLT_MIRROR_SETTING;

#define WM_FLT_MIRROR_GETVALUE     (WM_APP + 42)
#define WM_FLT_MIRROR_SETVALUE     (WM_APP + 142)
#define WM_FLT_MIRROR_CHANGEVALUE  (WM_APP + 242)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in ProgramList.cpp
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    PRESWITCHMUTEDELAY,
    POSTSWITCHMUTEDELAY,
    ANTIPLOP_SETTING_LASTONE,
} ANTIPLOP_SETTING;

#define WM_ANTIPLOP_GETVALUE     (WM_APP + 43)
#define WM_ANTIPLOP_SETVALUE     (WM_APP + 143)
#define WM_ANTIPLOP_CHANGEVALUE  (WM_APP + 243)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_TOMSMOCOMP.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    TOMSMOCOMPSEARCHEFFORT = 0,
    TOMSMOCOMPUSESTRANGEBOB,
    DI_TOMSMOCOMP_SETTING_LASTONE,
} DI_TOMSMOCOMP_SETTING;

#define WM_DI_TOMSMOCOMP_GETVALUE     (WM_APP + 44)
#define WM_DI_TOMSMOCOMP_SETVALUE     (WM_APP + 144)
#define WM_DI_TOMSMOCOMP_CHANGEVALUE  (WM_APP + 244)


/////////////////////////////////////////////////////////////////////////////
// Control settings for save per channel option
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    SETTINGSPERCHANNEL_BYCHANNEL = 0,
    SETTINGSPERCHANNEL_BYINPUT,
    SETTINGSPERCHANNEL_BYFORMAT,
    SETTINGSPERCHANNEL_SETTING_LASTONE,
} SETTINGSPERCHANNEL_SETTING;

#define WM_SETTINGSPERCHANNEL_GETVALUE     (WM_APP + 45)
#define WM_SETTINGSPERCHANNEL_SETVALUE     (WM_APP + 145)
#define WM_SETTINGSPERCHANNEL_CHANGEVALUE  (WM_APP + 245)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_Mirror.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USEINVERSION,
    FLT_INVERSION_SETTING_LASTONE,
} FLT_INVERSION_SETTING;

#define WM_FLT_INVERSION_GETVALUE     (WM_APP + 46)
#define WM_FLT_INVERSION_SETVALUE     (WM_APP + 146)
#define WM_FLT_INVERSION_CHANGEVALUE  (WM_APP + 246)


/////////////////////////////////////////////////////////////////////////////
// Control settings contained in CX2388XSource.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    CX2388XBRIGHTNESS = 0,
    CX2388XCONTRAST,
    CX2388XHUE,
    CX2388XSATURATION,
    CX2388XSATURATIONU,
    CX2388XSATURATIONV,
    CX2388XTOPOVERSCAN,
    CX2388XVIDEOSOURCE,
    CX2388XTVFORMAT,
    CX2388XCARDTYPE,
    CX2388XTUNERTYPE,
    CX2388XSAVEPERINPUT_DISABLED,
    CX2388XSAVEPERFORMAT_DISABLED,
    CX2388XSAVETVFORMATPERINPUT_DISABLED,
    CX2388XISINPUTPROGRESSIVE,
    CX2388XFLIFILMDETECT,
    CX2388XHDELAY,
    CX2388XVDELAY,
    CX2388XEATLINESATTOP,
    CX2388XSHARPNESS,
    CX2388XLUMAAGC,
    CX2388XCHROMAAGC,
    CX2388XFASTSUBCARRIERLOCK,
    CX2388XWHITECRUSH,
    CX2388XLOWCOLORREMOVAL,
    CX2388XCOMBFILTER,
    CX2388XFULLLUMARANGE,
    CX2388XREMODULATION,
    CX2388XCHROMA2HCOMB,
    CX2388XFORCEREMODEXCESSCHROMA,
    CX2388XIFXINTERPOLATION,
    CX2388XCOMBRANGE,
    CX2388XSECONDCHROMADEMOD,
    CX2388XTHIRDCHROMADEMOD,
    CX2388XPIXELWIDTH,
    CX2388XCUSTOMPIXELWIDTH,
    CX2388XWHITECRUSHUP,
    CX2388XWHITECRUSHDOWN,
    CX2388XWHITECRUSHMAJSEL,
    CX2388XWHITECRUSHPERFRAME,
    CX2388XVOLUME,
    CX2388XBALANCE,
    CX2388XAUDIOTYPE,
    CX2388XSTEREOTYPE,
    CX2388XBOTTOMOVERSCAN,
    CX2388XLEFTOVERSCAN,
    CX2388XRIGHTOVERSCAN,
    CX2388XANALOGUEBLANKING,
    CX2388XSTARTSTOPDRIVER,
    CX2388XAUTOMUTEIFNOTUNERSIGNAL,
    CX2388X_SETTING_LASTONE,
} CX2388X_SETTING;

#define WM_CX2388X_GETVALUE           (WM_APP + 47)
#define WM_CX2388X_SETVALUE           (WM_APP + 147)
#define WM_CX2388X_CHANGEVALUE        (WM_APP + 247)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FD_Prog.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    PROGTHRESHHOLDSTILL = 0,
    PROGTHRESHHOLDMOTION,
    PROGREPEATCOUNT,
    FDPROG_SETTING_LASTONE,
} FDPROG_SETTING;

#define WM_FDPROG_GETVALUE            (WM_APP + 48)
#define WM_FDPROG_SETVALUE            (WM_APP + 148)
#define WM_FDPROG_CHANGEVALUE         (WM_APP + 248)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in SAA7134Source.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    SAA7134BRIGHTNESS = 0,
    SAA7134CONTRAST,
    SAA7134SATURATION,
    SAA7134HUE,
    SAA7134TOPOVERSCAN,
    SAA7134PIXELWIDTH,
    SAA7134CUSTOMPIXELWIDTH,
    SAA7134VIDEOSOURCE,
    SAA7134TVFORMAT,
    SAA7134REVERSEPOLARITY,
    SAA7134CARDTYPE,
    SAA7134TUNERTYPE,
    SAA7134HPLLMODE,
    SAA7134WHITEPEAK,
    SAA7134COLORPEAK,
    SAA7134ADAPTIVECOMBFILTER,
    SAA7134HDELAY,
    SAA7134VDELAY,
    SAA7134AUDIOSTANDARD,
    SAA7134AUDIOCHANNEL,
    SAA7134AUDIOSAMPLERATE,
    SAA7134AUTOSTEREOSELECT,
    SAA7134VOLUME,
    SAA7134BASS,
    SAA7134TREBLE,
    SAA7134BALANCE,
    SAA7134AUDIOSOURCE,
    SAA7134CUSTOMAUDIOSTANDARD,
    SAA7134AUDIOSTANDARDCARRIER1,
    SAA7134AUDIOSTANDARDCARRIER2,
    SAA7134AUDIOSTANDARDCARRIER1MODE,
    SAA7134AUDIOSTANDARDCARRIER2MODE,
    SAA7134AUDIOSTANDARDCH1FMDEEMPH,
    SAA7134AUDIOSTANDARDCH2FMDEEMPH,
    SAA7134AUTOMATICVOLUMELEVEL,
    SAA7134VBIUPSCALEDIVISOR,
    SAA7134VBIDEBUGOVERLAY,
    SAA7134AUTOMATICGAINCONTROL,
    SAA7134GAINCONTROLLEVEL,
    SAA7134VIDEOMIRROR,
    SAA7134AUDIOLINE1VOLTAGE,
    SAA7134AUDIOLINE2VOLTAGE,
    SAA7134BOTTOMOVERSCAN,
    SAA7134LEFTOVERSCAN,
    SAA7134RIGHTOVERSCAN,
    SAA7134_SETTING_LASTONE,
} SAA7134_SETTING;

#define WM_SAA7134_GETVALUE           (WM_APP + 49)
#define WM_SAA7134_SETVALUE           (WM_APP + 149)
#define WM_SAA7134_CHANGEVALUE        (WM_APP + 249)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in ToolbarControl.cpp
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    TOOLBARS_SHOWTOOLBAR1 = 0,
    TOOLBARS_TOOLBAR1POSITION = 1,
    TOOLBARS_TOOLBARCHILD_POSITION = 2,
    TOOLBARS_TOOLBARCHILD_LASTONE  = 42,
    TOOLBARS_SETTING_LASTONE,
} TOOLBARS_SETTING;

#define WM_TOOLBARS_GETVALUE            (WM_APP + 50)
#define WM_TOOLBARS_SETVALUE            (WM_APP + 150)
#define WM_TOOLBARS_CHANGEVALUE         (WM_APP + 250)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in DI_MOCOMP2.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    MOCOMP2SEARCHEFFORT = 0,
    DI_MOCOMP2_SETTING_LASTONE,
} DI_MOCOMP2_SETTING;

#define WM_DI_MOCOMP2_GETVALUE     (WM_APP + 51)
#define WM_DI_MOCOMP2_SETVALUE     (WM_APP + 151)
#define WM_DI_MOCOMP2_CHANGEVALUE  (WM_APP + 251)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_Chroma.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    CHROMAHORZSHIFT = 0,
    CHROMAVERTSHIFT,
    LOWPASSCHROMAFILTER,
    USECHROMAFILTER,
    FLT_CHROMA_SETTING_LASTONE,
} FLT_CHROMA_SETTING;

#define WM_FLT_CHROMA_GETVALUE       (WM_APP + 52)
#define WM_FLT_CHROMA_SETVALUE       (WM_APP + 152)
#define WM_FLT_CHROMA_CHANGEVALUE    (WM_APP + 252)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_BlackWhite.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USEBLACKWHITEFILTER = 0,
    FLT_BLACKWHITE_SETTING_LASTONE,
} FLT_BLACKWHITE_SETTING;

#define WM_FLT_BLACKWHITE_GETVALUE       (WM_APP + 53)
#define WM_FLT_BLACKWHITE_SETVALUE       (WM_APP + 153)
#define WM_FLT_BLACKWHITE_CHANGEVALUE    (WM_APP + 253)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_DScopeVIDEO.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USE_DSVIDEO = 0,
	DSV_LINE,
    DSV_PLOTIINTENSITYINVERT,
    DSV_Y_SCALE,
    DSV_Y1_BASELINE,
    USE_GRID,
    DSV_GRIDCOLOR,
    DSV_GRIDINTENSITY,
    USE_BACKGROUND,
    USE_LINEMARKER,
    USE_PICTUREFILL,
    FLT_DSV_SETTING_LASTONE,
} FLT_DSV_SETTING;

#define WM_FLT_DSV_GETVALUE     (WM_APP + 54)
#define WM_FLT_DSV_SETVALUE     (WM_APP + 154)
#define WM_FLT_DSV_CHANGEVALUE  (WM_APP + 254)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_LuminChromaShift.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USELUMINCHROMASHIFT = 0,
	SHIFTVALUE,
    FLT_LUMINCHROMASHIFT_SETTING_LASTONE,
} FLT_LUMINCHROMASHIFT_SETTING;

#define WM_FLT_LUMINCHROMASHIFT_GETVALUE     (WM_APP + 55)
#define WM_FLT_LUMINCHROMASHIFT_SETVALUE     (WM_APP + 155)
#define WM_FLT_LUMINCHROMASHIFT_CHANGEVALUE  (WM_APP + 255)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in FLT_Colorimetry.c
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    USECOLORIMETRY = 0,
    COLORIMETRYTYPE,
    FLT_COLORIMETRY_SETTING_LASTONE,
} FLT_COLORIMETRY_SETTING;

#define WM_FLT_COLORIMETRY_GETVALUE       (WM_APP + 56)
#define WM_FLT_COLORIMETRY_SETVALUE       (WM_APP + 156)
#define WM_FLT_COLORIMETRY_CHANGEVALUE    (WM_APP + 256)

/////////////////////////////////////////////////////////////////////////////
// Control settings contained in TimeShift.cpp
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
	TIMESHIFTSAVINGPATH,
	TIMESHIFTSHOWWARNING,
	TIMESHIFTFILELIMIT,
    TIMESHIFT_SETTING_LASTONE,
} TIMESHIFT_SETTING;

#define WM_TIMESHIFT_GETVALUE		(WM_APP + 57)
#define WM_TIMESHIFT_SETVALUE		(WM_APP + 157)
#define WM_TIMESHIFT_CHANGEVALUE	(WM_APP + 257)

#endif
